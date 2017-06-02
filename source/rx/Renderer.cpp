// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <unordered_map>
#include <algorithm>


#include "rx.h"
#include "rx/model.h"

#include "platform.h"

#include "glwrapper.h"

#include "utf8rewind.h"

#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>

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
		this->projectionMatrix = glm::perspective(fov, this->_width / this->_height, near, far);

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

		this->setAmbientLighting(glm::vec4(1.0), 0.2);

		// one completely white pixel.
		static uint8_t white[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		this->placeholderTexture = new Texture(new Surface(white, 1, 1, ImageFormat::RGBA), this);
	}





	// misc shit
	RenderCommand::RenderCommand()
	{
		static id_t __id = 1;
		this->id = __id++;
	}

	glm::vec3 Camera::front() const
	{
		return glm::normalise(glm::vec3(
			cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw)),
			sin(glm::radians(this->pitch)),
			cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw)))
		);
	}

	glm::vec3 Camera::right() const
	{
		return glm::normalize(glm::cross(this->front(), glm::vec3(0, 1, 0)));
	}

	glm::vec3 Camera::up() const
	{
		return glm::normalise(glm::vec3(
			sin(glm::radians(this->roll)),
			cos(glm::radians(this->roll)),
			0.0
		));
	}


	void Renderer::updateCamera(const Camera& cam)
	{
		this->camera = cam;
		this->cameraMatrix = glm::lookAt(cam.position, cam.position + cam.front(), cam.up());
	}

	Camera Renderer::getCamera()
	{
		return this->camera;
	}

	void Renderer::clearRenderList()
	{
		this->renderList.clear();
	}

	void Renderer::clearScreen(util::colour colour)
	{
		RenderCommand rc;
		rc.type = RenderCommand::CommandType::Clear;
		rc.colours.push_back(colour);

		this->renderList.push_back(rc);
	}

	void Renderer::updateWindowSize(double width, double height)
	{
		this->_width = width;
		this->_height = height;

		this->window->width = width;
		this->window->height = height;

		LOG("window resized to %d x %d", (int) this->_width, (int) this->_height);

		// redo the projection matrix
		this->projectionMatrix = glm::perspective(this->_fov, this->_width / this->_height, this->_near, this->_far);

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

	void Renderer::setAmbientLighting(glm::vec4 colour, float intensity)
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

	void Renderer::renderColouredVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals)
	{
		RenderCommand rc;
		rc.type			= RenderCommand::CommandType::RenderColouredVertices;
		rc.wireframe	= false;
		rc.vertices		= verts;
		rc.colours		= colours;
		rc.normals		= normals;
		rc.uvs			= { };

		rc.dimensions = 3;
		rc.isInScreenSpace = false;

		this->renderList.push_back(rc);
	}

	void Renderer::renderStringInNormalisedScreenSpace(std::string txt, rx::Font* font, float size, glm::vec2 pos, TextAlignment align)
	{
		pos.x *= this->_width;
		pos.y *= this->_height;

		this->renderStringInScreenSpace(txt, font, size, pos);
	}

	void Renderer::renderStringInScreenSpace(std::string str, rx::Font* font, float size, glm::vec2 pos, TextAlignment align)
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
		double xPos = glm::round(pos).x;

		// (0, 0) in font-space is the bottom left, because :shrug:, so flip it.
		double yPos = glm::round(pos).y + (scale * font->ascent);

		glm::vec4 cliprect;
		{
			double x0 = xPos;
			double y0 = yPos + (scale * font->descent);

			double advx = 0;
			for(size_t i = 0; i < codepoints.size(); i++)
				advx += scale * font->getGlyphMetrics(codepoints[i]).xAdvance;

			// if we're right-aligned, move the xpos all the way to just before the last char
			if(rightAlign)
				xPos = this->_width - xPos - scale * font->getGlyphMetrics(codepoints.back()).xAdvance;

			double advy = scale * font->ascent;

			double x1 = x0 + advx;
			double y1 = y0 + advy;

			cliprect = { x0, y0, x1, y1 };
		}



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
					RenderCommand rc;

					rc.type = RenderCommand::CommandType::RenderText;
					rc.wireframe = false;

					rc.dimensions = 2;
					rc.isInScreenSpace = true;
					rc.textureToBind = font->glTextureID;

					// fill it up.
					auto pos = glm::vec2(round(xPos), round(yPos));

					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y0), 0, 1)));	// 3
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y1), 0, 1)));	// 2
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y0), 0, 1)));	// 1

					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y1), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y0), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y1), 0, 1)));

					rc.uvs = {
								glm::vec2(gpos.u1, gpos.v0),	// 3
								glm::vec2(gpos.u0, gpos.v1),	// 2
								glm::vec2(gpos.u0, gpos.v0),	// 1

								glm::vec2(gpos.u0, gpos.v1),
								glm::vec2(gpos.u1, gpos.v0),
								glm::vec2(gpos.u1, gpos.v1)
							};

					this->renderList.push_back(rc);
				}

				xPos += (rightAlign ? -1 : 1) * scale * gpos.xAdvance;
			}
		}
	}







	void Renderer::renderMesh(const Mesh& mesh, const Material& m, glm::mat4 transform)
	{
		Material mat = m;
		RenderCommand rc;

		auto placeholderColour = util::colour::white();

		if(!mat.diffuseMap)
		{
			placeholderColour = mat.diffuseColour;
			rc.type = RenderCommand::CommandType::RenderColouredVertices;
		}
		else
		{
			rc.type = RenderCommand::CommandType::RenderTexturedVertices;
		}

		rc.dimensions = 3;
		rc.isInScreenSpace = false;
		rc.material = mat;

		if(mesh.faces.empty())
			ERROR("mesh (id %zu) needs at least one face", mesh.id);

		for(auto face : mesh.faces)
		{
			if(face.vertices.empty())
				ERROR("face needs at least one vertex");

			for(auto v : face.vertices)
				rc.vertices.push_back(transform * glm::vec4(v, 1.0));

			for(auto t : face.uvs)
				rc.uvs.push_back(t);

			for(auto n : face.normals)
				rc.normals.push_back(n);

			// add some white colours
			rc.colours.insert(rc.colours.begin(), rc.vertices.size(), placeholderColour);
		}

		this->renderList.push_back(rc);
	}


	void Renderer::renderModel(const Model& model, glm::mat4 transform)
	{
		for(auto mesh : model.objects)
			this->renderMesh(mesh.first, mesh.second, transform);
	}



































	std::vector<PointLight> Renderer::sortAndUpdatePointLights(glm::vec3 vert)
	{
		// sort by distance, take the first N only.
		std::vector<PointLight> lights(this->pointLights.begin(), this->pointLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const PointLight& a, const PointLight& b) -> bool {
			return glm::distance(vert, a.position) < glm::distance(vert, b.position);
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




	std::vector<SpotLight> Renderer::sortAndUpdateSpotLights(glm::vec3 vert)
	{
		// sort by distance, take the first N only.
		std::vector<SpotLight> lights(this->spotLights.begin(), this->spotLights.end());
		std::sort(lights.begin(), lights.end(), [vert](const SpotLight& a, const SpotLight& b) -> bool {
			return glm::distance(vert, a.position) < glm::distance(vert, b.position);
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

		// generate one buffer, once.

		static GLuint uvBuffer = -1;
		static GLuint vertBuffer = -1;
		static GLuint colourBuffer = -1;
		static GLuint normalBuffer = -1;

		if(uvBuffer == (GLuint) -1)		glGenBuffers(1, &uvBuffer);
		if(vertBuffer == (GLuint) -1)	glGenBuffers(1, &vertBuffer);
		if(colourBuffer == (GLuint) -1)	glGenBuffers(1, &colourBuffer);
		if(normalBuffer == (GLuint) -1)	glGenBuffers(1, &normalBuffer);



		// render a cube at every point light
		for(auto pl : this->pointLights)
		{
			this->renderMesh(Mesh::getUnitCube(), Material(util::colour::white(), util::colour::white(), util::colour::white(), 1),
				glm::translate(glm::scale(glm::mat4(), glm::vec3(0.1)), pl.position));
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
			using CType = RenderCommand::CommandType;

			switch(rc.type)
			{
				case CType::Clear: {

					assert(rc.colours.size() == 1);
					auto col = rc.colours[0];

					glClearColor(col.r, col.g, col.b, col.a);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				} break;

				case CType::RenderColouredVertices: {

					auto& sprog = this->colourShaderProgram;
					sprog.use();

					sprog.setUniform("modelMatrix", glm::mat4(1.0));
					sprog.setUniform("viewMatrix", this->cameraMatrix);
					sprog.setUniform("projMatrix", this->projectionMatrix);

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// location
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// we must have colours
					assert(rc.colours.size() > 0);
					{
						glEnableVertexAttribArray(1);

						glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.colours.size() * sizeof(glm::vec4), &rc.colours[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// location
							4,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// if we have normals:
					if(rc.normals.size() > 0)
					{
						glEnableVertexAttribArray(2);

						glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.normals.size() * sizeof(glm::vec3), &rc.normals[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							2,			// location
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// if we have a material
					if(rc.material.hasValue)
					{
						auto& mat = rc.material;
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
						sprog.setUniform("material.ambientColour", glm::vec4(1.0));
						sprog.setUniform("material.diffuseColour", glm::vec4(1.0));
						sprog.setUniform("material.specularColour", glm::vec4(1.0));
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



					glDrawArrays(rc.wireframe ? GL_LINES : GL_TRIANGLES, 0, rc.vertices.size());

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);
					glDisableVertexAttribArray(2);

				} break;











				case CType::RenderTexturedVertices: {

					auto& sprog = this->textureShaderProgram;
					sprog.use();

					sprog.setUniform("modelMatrix", glm::mat4(1.0));
					sprog.setUniform("viewMatrix", this->cameraMatrix);
					sprog.setUniform("projMatrix", this->projectionMatrix);

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// location
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// we still need to upload a bunch of white colours, so our texture doesn't get multiplied by 0 and disappear
					// this should be handled by the mesh-maker, because it's not performant to do this every frame
					assert(rc.colours.size() > 0);
					{
						glEnableVertexAttribArray(1);

						glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.colours.size() * sizeof(glm::vec4), &rc.colours[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// location
							4,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// if we have normals:
					assert(rc.normals.size() > 0);
					{
						glEnableVertexAttribArray(2);

						glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.normals.size() * sizeof(glm::vec3), &rc.normals[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							2,			// location
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// uvs. the texture we're using depends on whether we have a material or not (see below)
					assert(rc.uvs.size() > 0);
					{
						glEnableVertexAttribArray(3);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							3,			// location
							2,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}


					auto& mat = rc.material;
					{
						// i presume this sets which texture unit to use
						sprog.setUniform("material.diffuseTexture", 0);
						sprog.setUniform("material.specularTexture", 1);

						sprog.setUniform("material.shine", mat.hasValue ? mat.shine : 32.0f);

						if(!mat.hasValue) assert(rc.textureToBind != (GLuint) -1);

						// diffuse map is compulsory
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, mat.hasValue ? mat.diffuseMap->glTextureID : rc.textureToBind);

						// specular is not
						if(mat.specularMap != 0)
							glActiveTexture(GL_TEXTURE1), glBindTexture(GL_TEXTURE_2D, mat.specularMap->glTextureID);

						glActiveTexture(GL_TEXTURE0);

						// set the colours so that multiplying by them does nothing.
						sprog.setUniform("material.ambientColour", glm::vec4(1.0));
						sprog.setUniform("material.diffuseColour", glm::vec4(1.0));
						sprog.setUniform("material.specularColour", glm::vec4(1.0));
					}




					glDrawArrays(rc.wireframe ? GL_LINES : GL_TRIANGLES, 0, rc.vertices.size());

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);
					glDisableVertexAttribArray(2);
					glDisableVertexAttribArray(3);

				} break;



				case CType::RenderText: {

					this->textShaderProgram.use();

					glm::mat4 orthoProj = glm::ortho(0.0, this->_width, this->_height, 0.0);
					this->textShaderProgram.setUniform("projectionMatrix", orthoProj);

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// location
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					GL::pushTextureBinding(rc.textureToBind);

					glEnableVertexAttribArray(1);
					{
						assert(rc.uvs.size() > 0);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// location
							2,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glDrawArrays(GL_TRIANGLES, 0, rc.vertices.size());

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

					GL::popTextureBinding();

				} break;

				case CType::Invalid:
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
		platform::preFrame(r->window->platformData, r->window->platformWindow);
	}

	void EndFrame(rx::Renderer* r)
	{
		r->renderAll();
		platform::endFrame(r->window->platformData, r->window->platformWindow);
	}






	#if 0


	// split the RenderDrawLists function into separate parts.
	// part 1 sets up the opengl stuff
	// part 2 does the rendering of the imgui drawlist
	// part 3 renders *OUR* things
	// part 4 finishes the opengl stuff

	void SetupOpenGL2D(ImDrawData* draw_data, int* fb_width, int* fb_height)
	{
		using namespace gl;

		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		// glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);


		glEnable(GL_TEXTURE_2D);
		// glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context


		// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
		ImGuiIO& io = ImGui::GetIO();
		*fb_width = (int) (io.DisplaySize.x * io.DisplayFramebufferScale.x);
		*fb_height = (int) (io.DisplaySize.y * io.DisplayFramebufferScale.y);


		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei) *fb_width, (GLsizei) *fb_height);

		// Setup orthographic projection matrix
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		// glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);

		double fov = glm::radians(70.0);
		double near = -1000;

		double top = tan(0.5 * fov) * near;
		double bottom = -1 * top;

		double aspect = (double) io.DisplaySize.x / (double) io.DisplaySize.y;

		double left = aspect * bottom;
		double right = aspect * top;

		glFrustum(left, right, bottom, top, -1000.0, 1000);
		// gluPerspective(60.0, (float) io.DisplaySize.x / (float) io.DisplaySize.y, 1.0, 50000000000.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	void RenderImGui(ImDrawData* draw_data, int fb_height)
	{
		glEnable(GL_SCISSOR_TEST);

		// Render command lists
		#define OFFSETOF(TYPE, ELEMENT) ((size_t) &(((TYPE*) 0)->ELEMENT))
		for(int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const unsigned char* vtx_buffer = (const unsigned char*) &cmd_list->VtxBuffer.front();
			const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();

			glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*) (vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*) (vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*) (vtx_buffer + OFFSETOF(ImDrawVert, col)));

			for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if(pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					// call into our own state handler
					// so we can keep track of shit.

					GL::pushTextureBinding((GLuint) (uintptr_t) pcmd->TextureId);


					glScissor((int) pcmd->ClipRect.x, (int) (fb_height - pcmd->ClipRect.w),
						(int) (pcmd->ClipRect.z - pcmd->ClipRect.x), (int) (pcmd->ClipRect.w - pcmd->ClipRect.y));

					glDrawElements(GL_TRIANGLES, (GLsizei) pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);


					GL::popTextureBinding();
				}
				idx_buffer += pcmd->ElemCount;
			}
		}
		#undef OFFSETOF
	}


	void FinishOpenGL2D()
	{
		// Restore modified state
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}



	void SetupOpenGL3D()
	{
		glEnable(GL_DEPTH_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);
	}

	void FinishOpenGL3D()
	{
	}
	#endif
}













	#if 0
	void Renderer::RenderCircle(Math::Circle circ, bool fill)
	{
		int32_t x = circ.radius;
		int32_t y = 0;
		double radiusError = 1.0 - x;

		// who needs trigo?
		while(x >= y)
		{
			auto nxpy = Vector2(-x + circ.origin.x, +y + circ.origin.y);
			auto pxpy = Vector2(+x + circ.origin.x, +y + circ.origin.y);
			auto nypx = Vector2(-y + circ.origin.x, +x + circ.origin.y);
			auto pypx = Vector2(+y + circ.origin.x, +x + circ.origin.y);
			auto nynx = Vector2(-y + circ.origin.x, -x + circ.origin.y);
			auto pynx = Vector2(+y + circ.origin.x, -x + circ.origin.y);
			auto nxny = Vector2(-x + circ.origin.x, -y + circ.origin.y);
			auto pxny = Vector2(+x + circ.origin.x, -y + circ.origin.y);


			if(fill)
			{
				this->RenderLine(Vector2(nxpy), Vector2(pxpy));
				this->RenderLine(Vector2(nypx), Vector2(pypx));
				this->RenderLine(Vector2(nynx), Vector2(pynx));
				this->RenderLine(Vector2(nxny), Vector2(pxny));
			}
			else
			{
				this->RenderPoint(nxpy);
				this->RenderPoint(pxpy);

				this->RenderPoint(nypx);
				this->RenderPoint(pypx);

				this->RenderPoint(nynx);
				this->RenderPoint(pynx);

				this->RenderPoint(nxny);
				this->RenderPoint(pxny);
			}

			y++;

			if(radiusError < 0)
			{
				radiusError += 2.0 * y + 1.0;
			}
			else
			{
				x--;
				radiusError += 2.0 * (y - x + 1.0);
			}
		}
	}
	#endif













	#if 0
	void Renderer::renderStringRightAligned(std::string txt, rx::Font font, float size, Math::Vector2 pt)
	{
		// auto cmd = RenderCommand::createRenderString(txt, font, size, this->drawColour, Math::Vector2(0, pt.y));

		// // the starting position (top-left)
		// double sx = this->window->width - fabs(cmd.bounds.second.x - cmd.bounds.first.x) - pt.x;

		// // offset all vertices to the right by that amount.
		// for(size_t i = 0; i < cmd.vertices.size(); i += 4)
		// {
		// 	// 4 verts per character; first 2 are x and y, second 2 are u and v
		// 	auto& x = cmd.vertices[i + 0];
		// 	auto& y = cmd.vertices[i + 1];

		// 	x.x += sx;
		// 	y.x += sx;
		// }

		// cmd.bounds.first.x += sx;
		// cmd.bounds.second.x += sx;

		// this->renderList.push_back(cmd);
	}

	size_t Renderer::getStringWidthInPixels(std::string txt, rx::Font font, float size)
	{
		#if 0
		if(txt.empty()) return 0;

		// note: this is basically going through each glyph twice, so only use when necessary.
		auto cmd = RenderCommand::createRenderString(txt, font, size, this->drawColour, Math::Vector2(0, 0));
		return (size_t) fabs(cmd.bounds.second.x - cmd.bounds.first.x);
		#endif

		return 0;
	}
	#endif












