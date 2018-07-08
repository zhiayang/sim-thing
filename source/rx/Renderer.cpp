// Renderer.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>
#include <algorithm>
#include <unordered_map>


#include "rx.h"
#include "rx/model.h"

#include "platform.h"

#include "glwrapper.h"

#include "utf8rewind.h"

#include <glbinding/gl/gl.h>

using namespace gl;

#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 16

namespace rx
{
	Renderer::Renderer(Window* win, util::colour clearCol, const Camera& cam, const ShaderPipeline& pipe, double fov, double near, double far)
		: pipeline(pipe)
	{
		assert(win);
		this->window = win;

		this->_width			= window->width;
		this->_height			= window->height;
		this->_resolutionScale	= window->displayScale;

		// identity matrix.
		this->updateCamera(cam);
		this->projectionMatrix = lx::perspective(fov, this->_width / this->_height, near, far);

		this->clearColour = clearCol;

		this->_fov		= fov;
		this->_near		= near;
		this->_far		= far;


		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// glEnable(GL_FRAMEBUFFER_SRGB);
		// glEnable(GL_MULTISAMPLE);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		this->setAmbientLighting(util::colour::white(), 0.2);

		// one completely white pixel.
		static uint8_t white[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		this->placeholderTexture = new Texture(white, 1, 1, ImageFormat::RGBA);


		glClearColor(this->clearColour.r, this->clearColour.g, this->clearColour.b, this->clearColour.a);
	}





	// misc shit
	RenderCommand::RenderCommand()
	{
		static id_t __id = 1;
		this->id = __id++;
	}

	lx::vec3 Camera::front() const
	{
		return lx::vec3(
			lx::cos(lx::toRadians(this->pitch)) * lx::cos(lx::toRadians(this->yaw)),
			lx::sin(lx::toRadians(this->pitch)),
			lx::cos(lx::toRadians(this->pitch)) * lx::sin(lx::toRadians(this->yaw))
		).normalised();
	}

	lx::vec3 Camera::right() const
	{
		return lx::cross(this->front(), lx::vec3(0, 1, 0)).normalised();
	}

	lx::vec3 Camera::up() const
	{
		return lx::vec3(
			lx::sin(lx::toRadians(this->roll)),
			lx::cos(lx::toRadians(this->roll)),
			0.0
		).normalised();
	}


	void Renderer::updateCamera(const Camera& cam)
	{
		this->camera = cam;
		this->cameraMatrix = lx::lookAt(lx::vec3(cam.position), lx::vec3(cam.position + cam.front()), lx::vec3(cam.up()));
	}

	Camera Renderer::getCamera()
	{
		return this->camera;
	}

	void Renderer::clearRenderList()
	{
		this->forwardList.clear();
	}

	void Renderer::updateWindowSize(double width, double height)
	{
		this->_width = width;
		this->_height = height;

		this->window->width = width;
		this->window->height = height;

		LOG("window resized to %d x %d", (int) this->_width, (int) this->_height);

		// redo the projection matrix
		this->projectionMatrix = lx::perspective(this->_fov, this->_width / this->_height, this->_near, this->_far);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		double resscale = this->_resolutionScale;
		glViewport(0, 0, (int) (this->_width * resscale), (int) (this->_height * resscale));

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}










	// lighting

	void Renderer::setAmbientLighting(const util::colour& colour, float intensity)
	{
		this->pipeline.shaders[0].setUniform("ambientLightColour", colour);
		this->pipeline.shaders[0].setUniform("ambientLightIntensity", intensity);
	}

	void Renderer::addPointLight(const rx::PointLight& light)
	{
		this->pointLights.push_back(light);
	}

	void Renderer::addSpotLight(const rx::SpotLight& light)
	{
		this->spotLights.push_back(light);
	}
















	// render things
	void Renderer::renderObject(rx::RenderObject* ro, const lx::mat4& transform)
	{
		RenderCommand rc;
		rc.renderObject = ro;
		rc.modelMatrix = transform;

		if(!rc.renderObject->material.hasValue)
			rc.renderObject->material = Material(util::colour::white(), this->placeholderTexture, this->placeholderTexture, 1);

		this->forwardList.push_back(rc);
	}


	void Renderer::renderStringInNormalisedScreenSpace(const std::string& txt, rx::Font* font, float size, const lx::fvec2& _pos,
		const util::colour& colour, TextAlignment align)
	{
		auto pos = _pos;
		pos.x *= this->_width;
		pos.y *= this->_height;

		this->renderStringInScreenSpace(txt, font, size, pos, colour, align);
	}

	void Renderer::renderStringInScreenSpace(const std::string& str, rx::Font* font, float size, const lx::fvec2& pos,
		const util::colour& colour, TextAlignment align)
	{
		bool rightAlign = (align == TextAlignment::RightAligned);

		std::vector<uint32_t> codepoints;
		{
			size_t length = utf8toutf32(str.c_str(), str.size(), nullptr, 0, 0) / 4;
			codepoints.resize(length);

			int32_t errs = 0;
			utf8toutf32(str.c_str(), str.size(), &codepoints[0], length * 4, &errs);
			assert(errs == UTF8_ERR_NONE);
		}

		// todo: this produces one rendercommand per character,
		// might be slightly inefficient...

		// do the magic

		float scale = size / (float) font->pixelSize;
		double xPos = lx::round(pos).x;

		// (0, 0) in font-space is the bottom left, because :shrug:, so flip it.
		double yPos = lx::round(pos).y + (scale * font->ascent);

		lx::fvec4 cliprect;
		{
			float x0 = xPos;
			float y0 = yPos + (scale * font->descent);

			float advx = 0;
			for(size_t i = 0; i < codepoints.size(); i++)
				advx += scale * font->getGlyphMetrics(codepoints[i]).xAdvance;

			// if we're right-aligned, move the xpos all the way to just before the last char
			if(rightAlign)
				xPos = this->_width - xPos - scale * font->getGlyphMetrics(codepoints.back()).xAdvance;

			float advy = scale * font->ascent;

			float x1 = x0 + advx;
			float y1 = y0 + advy;

			cliprect = lx::fvec4(0, y0, x1, y1);
		}


		// std::vector<lx::vec3> vertices;
		// std::vector<lx::vec2> uvs;

		std::vector<size_t> indices;
		std::vector<lx::fvec2> positions;

		for(size_t i = 0; i < codepoints.size(); i++)
		{
			// if we're right aligned, flip the sequence of iteration.
			uint32_t codepoint = (rightAlign ? codepoints[codepoints.size() - i - 1] : codepoints[i]);

			if(codepoint < ' ')
			{
				if(codepoint == '\n')
				{
					// line height is just the pixel height; increment
					yPos += (rightAlign ? -1 : 1) * font->pixelSize;
					continue;
				}
			}
			else
			{
				auto gpos = font->getGlyphMetrics(codepoint);

				if(codepoint != ' ')
				{
					// fill it up.
					auto pos = lx::fvec2(round(xPos), round(yPos));

					indices.push_back(codepoint - font->firstChar);
					positions.push_back(pos);
				}

				xPos += (rightAlign ? -1 : 1) * scale * gpos.xAdvance;
			}
		}

		RenderCommand rc;
		rc.renderObject = font->renderObject;
		rc.positions = positions;
		rc.indices = indices;
		rc.textScale = scale;
		rc.textColour = colour;

		this->forwardList.push_back(rc);
	}









































	std::vector<PointLight> Renderer::sortAndUpdatePointLights(const lx::fvec3& vert)
	{
		// sort by distance, take the first N only.
		std::vector<PointLight> lights(this->pointLights.begin(), this->pointLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const PointLight& a, const PointLight& b) -> bool {
			return lx::distance(vert, a.position) < lx::distance(vert, b.position);
		});

		if(lights.size() > MAX_POINT_LIGHTS)
			lights.erase(lights.begin() + MAX_POINT_LIGHTS, lights.end());

		{
			this->pipeline.shaders[0].use();

			size_t ctr = 0;
			for(auto light : lights)
			{
				std::string arraypre = "pointLights[" + std::to_string(ctr) + "].";

				this->pipeline.shaders[0].setUniform(arraypre + "position", light.position);
				this->pipeline.shaders[0].setUniform(arraypre + "intensity", light.intensity);

				this->pipeline.shaders[0].setUniform(arraypre + "diffuseColour", light.diffuseColour);
				this->pipeline.shaders[0].setUniform(arraypre + "specularColour", light.specularColour);

				this->pipeline.shaders[0].setUniform(arraypre + "lightRadius", light.lightRadius);

				ctr++;
			}

			this->pipeline.shaders[0].setUniform("pointLightCount", (int) ctr);
		}

		return lights;
	}




	std::vector<SpotLight> Renderer::sortAndUpdateSpotLights(const lx::fvec3& vert)
	{
		// sort by distance, take the first N only.
		std::vector<SpotLight> lights(this->spotLights.begin(), this->spotLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const SpotLight& a, const SpotLight& b) -> bool {
			return lx::distance(vert, a.position) < lx::distance(vert, b.position);
		});

		if(lights.size() > MAX_SPOT_LIGHTS)
			lights.erase(lights.begin() + MAX_SPOT_LIGHTS, lights.end());


		{
			this->pipeline.shaders[0].use();

			size_t ctr = 0;
			for(auto light : lights)
			{
				std::string arraypre = "spotLights[" + std::to_string(ctr) + "].";

				this->pipeline.shaders[0].setUniform(arraypre + "position", light.position);
				this->pipeline.shaders[0].setUniform(arraypre + "direction", light.direction);

				this->pipeline.shaders[0].setUniform(arraypre + "intensity", light.intensity);

				this->pipeline.shaders[0].setUniform(arraypre + "diffuseColour", light.diffuseColour);
				this->pipeline.shaders[0].setUniform(arraypre + "specularColour", light.specularColour);

				this->pipeline.shaders[0].setUniform(arraypre + "innerCutoffCosine", light.innerCutoffCosine);
				this->pipeline.shaders[0].setUniform(arraypre + "outerCutoffCosine", light.outerCutoffCosine);

				this->pipeline.shaders[0].setUniform(arraypre + "lightRadius", light.lightRadius);

				ctr++;
			}

			this->pipeline.shaders[0].setUniform("spotLightCount", (int) ctr);
		}

		return lights;
	}









	// main render pusher
	void Renderer::renderForward()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render a cube at every point light
		static auto cubeRO = RenderObject::fromMesh(Mesh::getUnitCube(), Material(util::colour::white(),
			util::colour::white(), util::colour::white(), 1));

		this->sortAndUpdatePointLights(tof(this->camera.position));
		this->sortAndUpdateSpotLights(tof(this->camera.position));


		for(auto pl : this->pointLights)
		{
			this->renderObject(cubeRO, lx::scale(0.1).translated(fromf(pl.position)));
		}

		for(auto rc : this->forwardList)
		{
			assert(rc.renderObject);
			auto renderObj = rc.renderObject;

			glBindVertexArray(renderObj->vertexArrayObject);

			switch(renderObj->renderType)
			{
				case RenderType::Vertices: {

					auto& sprog = this->pipeline.shaders[renderObj->shaderProgramIndex];
					sprog.use();

					sprog.setUniform("modelMatrix", tof(rc.modelMatrix));
					sprog.setUniform("viewMatrix", tof(this->cameraMatrix));
					sprog.setUniform("projMatrix", tof(this->projectionMatrix));

					if(sprog.capabilities & SHADER_SUPPORTS_MATERIALS)
					{
						assert(renderObj->material.hasValue);
						{
							auto& mat = renderObj->material;
							sprog.setUniform("material.shine", mat.shine);

							// set the colours
							sprog.setUniform("material.ambientColour", tof(mat.ambientColour));
							sprog.setUniform("material.diffuseColour", tof(mat.diffuseColour));
							sprog.setUniform("material.specularColour", tof(mat.specularColour));
						}

						{
							// i presume this sets which texture unit to use
							sprog.setUniform("material.diffuseTexture", 0);
							sprog.setUniform("material.specularTexture", 1);

							auto& mat = renderObj->material;
							Texture* diff = (mat.diffuseMap ? mat.diffuseMap : this->placeholderTexture);
							Texture* spec = (mat.specularMap ? mat.specularMap : this->placeholderTexture);

							// use the placeholder white texture
							glActiveTexture(GL_TEXTURE1);
							glBindTexture(GL_TEXTURE_2D, spec->glTextureID);

							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, diff->glTextureID);
						}
					}
					if(sprog.capabilities & SHADER_SUPPORTS_CAMERA_POSITION)
					{
						sprog.setUniform("cameraPosition", tof(this->camera.position));
					}

					glDrawArrays(renderObj->wireframe ? GL_LINES : GL_TRIANGLES, 0, renderObj->arrayLength);

				} break;




				case RenderType::Text: {

					this->pipeline.textShader.use();

					lx::mat4 orthoProj = lx::orthographic(0.0, this->_width, this->_height, 0.0);
					this->pipeline.textShader.setUniform("projectionMatrix", tof(orthoProj));
					this->pipeline.textShader.setUniform("fontScale", rc.textScale);
					this->pipeline.textShader.setUniform("fontColour", rc.textColour);

					glBindTexture(GL_TEXTURE_2D, renderObj->material.diffuseMap->glTextureID);

					for(size_t i = 0; i < rc.indices.size(); i++)
					{
						this->pipeline.textShader.setUniform("offsetPosition", rc.positions[i]);
						glDrawArrays(GL_TRIANGLES, rc.indices[i] * 6, 6);
					}

					glBindTexture(GL_TEXTURE_2D, 0);

				} break;


				case RenderType::Invalid:
					ERROR("Invalid render command type");
			}
		}

		this->clearRenderList();
	}

















	// imgui stuff
	void PreFrame(rx::Renderer* r)
	{
		platform::preFrame(r->window->platformData, r->window->platformWindow);
	}

	void BeginFrame(rx::Renderer* r)
	{
		platform::beginFrame(r->window->platformData, r->window->platformWindow);
	}

	void EndFrame(rx::Renderer* r)
	{
		r->renderForward();
		platform::endFrame(r->window->platformData, r->window->platformWindow);
	}
}



















