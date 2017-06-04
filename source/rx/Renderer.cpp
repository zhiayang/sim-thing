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
	Renderer::Renderer(Window* win, util::colour clearCol, Camera cam, ShaderProgram textureShaderProg,
		ShaderProgram colourShaderProg, ShaderProgram textShaderProg, double fov, double near, double far) :
		textureShaderProgram(textureShaderProg), colourShaderProgram(colourShaderProg), textShaderProgram(textShaderProg)
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

		// bind permanently...?
		gl::GLuint vertexArrayID;
		gl::glGenVertexArrays(1, &vertexArrayID);
		gl::glBindVertexArray(vertexArrayID);


		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		this->setAmbientLighting(lx::vec4(1.0), 0.2);

		// one completely white pixel.
		static uint8_t white[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		this->placeholderTexture = new Texture(white, 1, 1, ImageFormat::RGBA);
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
		this->renderList.clear();
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

	void Renderer::setAmbientLighting(lx::vec4 colour, float intensity)
	{
		// set for both
		for(auto* prog : { &this->colourShaderProgram, &this->textureShaderProgram })
		{
			prog->use();
			prog->setUniform("ambientLightColour", colour);
			prog->setUniform("ambientLightIntensity", intensity);
		}
	}

	void Renderer::addPointLight(rx::PointLight light)
	{
		this->pointLights.push_back(light);
	}

	void Renderer::addSpotLight(rx::SpotLight light)
	{
		this->spotLights.push_back(light);
	}
















	// render things
	void Renderer::renderObject(rx::RenderObject* ro, const lx::mat4& transform)
	{
		RenderCommand rc;
		rc.renderObject = ro;
		rc.modelMatrix = transform;

		this->renderList.push_back(rc);
	}


	void Renderer::renderStringInNormalisedScreenSpace(std::string txt, rx::Font* font, float size, lx::vec2 pos,
		util::colour colour, TextAlignment align)
	{
		pos.x *= this->_width;
		pos.y *= this->_height;

		this->renderStringInScreenSpace(txt, font, size, pos, colour, align);
	}

	void Renderer::renderStringInScreenSpace(std::string str, rx::Font* font, float size, lx::vec2 pos,
		util::colour colour, TextAlignment align)
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

		lx::vec4 cliprect;
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

			cliprect = lx::vec4(0, y0, x1, y1);
		}


		// std::vector<lx::vec3> vertices;
		// std::vector<lx::vec2> uvs;

		std::vector<size_t> indices;
		std::vector<lx::vec2> positions;

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
					auto pos = lx::vec2(round(xPos), round(yPos));

					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x1, gpos.y0), 0, 1)).xyz());	// 3
					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x0, gpos.y1), 0, 1)).xyz());	// 2
					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x0, gpos.y0), 0, 1)).xyz());	// 1

					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x0, gpos.y1), 0, 1)).xyz());
					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x1, gpos.y0), 0, 1)).xyz());
					// vertices.push_back(lx::round(lx::vec4(pos + scale * lx::vec2(gpos.x1, gpos.y1), 0, 1)).xyz());

					// uvs.insert(uvs.end(), {
					// 			lx::vec2(gpos.u1, gpos.v0),	// 3
					// 			lx::vec2(gpos.u0, gpos.v1),	// 2
					// 			lx::vec2(gpos.u0, gpos.v0),	// 1

					// 			lx::vec2(gpos.u0, gpos.v1),
					// 			lx::vec2(gpos.u1, gpos.v0),
					// 			lx::vec2(gpos.u1, gpos.v1)
					// 		});

					indices.push_back(codepoint - font->firstChar);
					positions.push_back(pos);
				}

				xPos += (rightAlign ? -1 : 1) * scale * gpos.xAdvance;
			}
		}

		// // auto normals = std::vector<lx::vec3>(vertices.size(), lx::vec3(0, 0, 1));
		// auto renderObject = RenderObject::fromTexturedVertices(vertices, uvs, { });

		// renderObject->material = Material(util::colour::white(), font->fontAtlas, font->fontAtlas, 32);
		// renderObject->renderType = RenderType::Text;

		// RenderCommand rc;
		// rc.renderObject = renderObject;

		// this->renderList.push_back(rc);
		// this->autoGeneratedRenderObjects.push_back(renderObject);

		RenderCommand rc;
		rc.renderObject = font->renderObject;
		rc.positions = positions;
		rc.indices = indices;
		rc.textScale = scale;
		rc.textColour = colour.vec4();

		this->renderList.push_back(rc);
	}









































	std::vector<PointLight> Renderer::sortAndUpdatePointLights(lx::vec3 vert)
	{
		// sort by distance, take the first N only.
		std::vector<PointLight> lights(this->pointLights.begin(), this->pointLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const PointLight& a, const PointLight& b) -> bool {
			return lx::distance(vert, a.position) < lx::distance(vert, b.position);
		});

		if(lights.size() > MAX_POINT_LIGHTS)
			lights.erase(lights.begin() + MAX_POINT_LIGHTS, lights.end());

		for(auto* shaderProg : { &this->colourShaderProgram, &this->textureShaderProgram })
		{
			assert(shaderProg);
			shaderProg->use();

			size_t ctr = 0;
			for(auto light : lights)
			{
				std::string arraypre = "pointLights[" + std::to_string(ctr) + "].";

				shaderProg->setUniform(arraypre + "position", light.position);
				shaderProg->setUniform(arraypre + "intensity", light.intensity);

				shaderProg->setUniform(arraypre + "diffuseColour", light.diffuseColour);
				shaderProg->setUniform(arraypre + "specularColour", light.specularColour);

				shaderProg->setUniform(arraypre + "lightRadius", light.lightRadius);

				ctr++;
			}

			shaderProg->setUniform("pointLightCount", (int) ctr);
		}

		return lights;
	}




	std::vector<SpotLight> Renderer::sortAndUpdateSpotLights(lx::vec3 vert)
	{
		// sort by distance, take the first N only.
		std::vector<SpotLight> lights(this->spotLights.begin(), this->spotLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const SpotLight& a, const SpotLight& b) -> bool {
			return lx::distance(vert, a.position) < lx::distance(vert, b.position);
		});

		if(lights.size() > MAX_SPOT_LIGHTS)
			lights.erase(lights.begin() + MAX_SPOT_LIGHTS, lights.end());

		for(auto* shaderProg : { &this->colourShaderProgram, &this->textureShaderProgram })
		{
			assert(shaderProg);
			shaderProg->use();

			size_t ctr = 0;
			for(auto light : lights)
			{
				std::string arraypre = "spotLights[" + std::to_string(ctr) + "].";

				shaderProg->setUniform(arraypre + "position", light.position);
				shaderProg->setUniform(arraypre + "direction", light.direction);

				shaderProg->setUniform(arraypre + "intensity", light.intensity);

				shaderProg->setUniform(arraypre + "diffuseColour", light.diffuseColour);
				shaderProg->setUniform(arraypre + "specularColour", light.specularColour);

				shaderProg->setUniform(arraypre + "innerCutoffCosine", light.innerCutoffCosine);
				shaderProg->setUniform(arraypre + "outerCutoffCosine", light.outerCutoffCosine);

				shaderProg->setUniform(arraypre + "lightRadius", light.lightRadius);

				ctr++;
			}

			shaderProg->setUniform("spotLightCount", (int) ctr);
		}

		return lights;
	}









	// main render pusher

	/*
		note about the renderer + shaders:

		if rendering coloured vertices, ie. without textures, the material used will be based on colours.
		ie. you have ambientColour, diffuseColour, specularColour.

		if rendering textured vertices, then the material used will be based on textures, and we need to upload
		UV coords to the shader.

		I don't think we'll support textured objects with colour-mats, since you can just use the same texture for the diffuse
		map. and using textures for colour-verts defeats the purpose (read: keep it simple) of the entire thing.

		so there. mutual exclusion by design.
	*/

	void Renderer::renderAll()
	{
		glClearColor(this->clearColour.r, this->clearColour.g, this->clearColour.b, this->clearColour.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double rscale = this->_resolutionScale;
		glViewport(0, 0, (int) (this->_width * rscale), (int) (this->_height * rscale));

		// render a cube at every point light
		static auto cubeRO = RenderObject::fromMesh(Mesh::getUnitCube(), Material(util::colour::white(),
			util::colour::white(), util::colour::white(), 1));

		for(auto pl : this->pointLights)
		{
			this->renderObject(cubeRO, lx::scale(0.1).translate(pl.position));
		}

		// sort the lights by distance to the camera
		{
			this->textureShaderProgram.setUniform("cameraPosition", this->camera.position);
			this->colourShaderProgram.setUniform("cameraPosition", this->camera.position);

			this->sortAndUpdatePointLights(this->camera.position);
			this->sortAndUpdateSpotLights(this->camera.position);
		}



		for(auto rc : this->renderList)
		{
			assert(rc.renderObject);
			auto renderObj = rc.renderObject;

			glBindVertexArray(renderObj->vertexArrayObject);

			switch(renderObj->renderType)
			{
				case RenderType::Coloured: {

					auto& sprog = this->colourShaderProgram;
					sprog.use();

					sprog.setUniform("modelMatrix", rc.modelMatrix);
					sprog.setUniform("viewMatrix", this->cameraMatrix);
					sprog.setUniform("projMatrix", this->projectionMatrix);

					// if we have a material
					if(renderObj->material.hasValue)
					{
						auto& mat = renderObj->material;
						sprog.setUniform("material.shine", mat.shine);

						// set the colours
						sprog.setUniform("material.ambientColour", mat.ambientColour);
						sprog.setUniform("material.diffuseColour", mat.diffuseColour);
						sprog.setUniform("material.specularColour", mat.specularColour);
					}
					else
					{
						// setup default material, which allows us to continue using the vertex colours.
						sprog.setUniform("material.shine", 32.0f);

						// set the colours
						sprog.setUniform("material.ambientColour", lx::vec4(1.0));
						sprog.setUniform("material.diffuseColour", lx::vec4(1.0));
						sprog.setUniform("material.specularColour", lx::vec4(1.0));
					}


					{
						// i presume this sets which texture unit to use
						sprog.setUniform("material.diffuseTexture", 0);
						sprog.setUniform("material.specularTexture", 1);

						// use the placeholder white texture
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, this->placeholderTexture->glTextureID);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, this->placeholderTexture->glTextureID);
					}

					glDrawArrays(renderObj->wireframe ? GL_LINES : GL_TRIANGLES, 0, renderObj->arrayLength);

				} break;











				case RenderType::Textured: {

					auto& sprog = this->textureShaderProgram;
					sprog.use();

					sprog.setUniform("modelMatrix", rc.modelMatrix);
					sprog.setUniform("viewMatrix", this->cameraMatrix);
					sprog.setUniform("projMatrix", this->projectionMatrix);

					auto& mat = renderObj->material;
					{
						// i presume this sets which texture unit to use
						sprog.setUniform("material.diffuseTexture", 0);
						sprog.setUniform("material.specularTexture", 1);

						sprog.setUniform("material.shine", mat.hasValue ? mat.shine : 32.0f);

						// diffuse map is compulsory
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, (mat.hasValue && mat.diffuseMap)
							? mat.diffuseMap->glTextureID : this->placeholderTexture->glTextureID);

						// specular is not
						if(mat.specularMap != 0)
							glActiveTexture(GL_TEXTURE1), glBindTexture(GL_TEXTURE_2D, mat.specularMap->glTextureID);

						glActiveTexture(GL_TEXTURE0);

						// set the colours so that multiplying by them does nothing.
						sprog.setUniform("material.ambientColour", lx::vec4(1.0));
						sprog.setUniform("material.diffuseColour", lx::vec4(1.0));
						sprog.setUniform("material.specularColour", lx::vec4(1.0));
					}

					glDrawArrays(renderObj->wireframe ? GL_LINES : GL_TRIANGLES, 0, renderObj->arrayLength);

				} break;



				case RenderType::Text: {

					this->textShaderProgram.use();

					lx::mat4 orthoProj = lx::orthographic(0.0, this->_width, this->_height, 0.0);
					this->textShaderProgram.setUniform("projectionMatrix", orthoProj);
					this->textShaderProgram.setUniform("fontScale", rc.textScale);
					this->textShaderProgram.setUniform("fontColour", rc.textColour);

					glBindTexture(GL_TEXTURE_2D, renderObj->material.diffuseMap->glTextureID);

					for(size_t i = 0; i < rc.indices.size(); i++)
					{
						this->textShaderProgram.setUniform("offsetPosition", rc.positions[i]);
						glDrawArrays(GL_TRIANGLES, rc.indices[i] * 6, 6);
					}

					glBindTexture(GL_TEXTURE_2D, 0);

				} break;

				case RenderType::Invalid:
					ERROR("Invalid render command type");
			}
		}

		this->clearRenderList();
		for(auto ro : this->autoGeneratedRenderObjects)
			delete ro;

		this->autoGeneratedRenderObjects.clear();
	}

















	// imgui stuff
	void PreFrame(rx::Renderer* r)
	{
		platform::preFrame(r->window->platformData, r->window->platformWindow);
	}

	void BeginFrame(rx::Renderer* r)
	{
		platform::preFrame(r->window->platformData, r->window->platformWindow);
	}

	void EndFrame(rx::Renderer* r)
	{
		r->renderAll();
		platform::endFrame(r->window->platformData, r->window->platformWindow);
	}





}












