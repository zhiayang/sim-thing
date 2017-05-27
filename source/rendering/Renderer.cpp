// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "glwrapper.h"
#include "renderer/rx.h"

#include "utf8rewind.h"

#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace gl;

namespace Rx
{
	Renderer::Renderer(Window* win, SDL_GLContext glc, util::colour clearCol, glm::mat4 camera, gl::GLuint textureShaderProg,
		gl::GLuint colourShaderProg, gl::GLuint textShaderProg, double fov, double width, double height, double resscale,
		double near, double far)
	{
		assert(win);
		this->window = win;
		this->glContext = glc;


		// identity matrix.
		this->cameraMatrix = camera;
		this->projectionMatrix = glm::perspective(fov, width / height, near, far);

		this->clearColour = clearCol;

		this->_fov		= fov;
		this->_width	= width;
		this->_height	= height;
		this->_near		= near;
		this->_far		= far;


		this->_resolutionScale = resscale;

		this->textureShaderProgram = textureShaderProg;
		this->colourShaderProgram = colourShaderProg;
		this->textShaderProgram = textShaderProg;


		// change to the program for a bit, so we can cache the uniform location.
		glUseProgram(this->textureShaderProgram);
		this->mvpMatrixId_textureShader = glGetUniformLocation(this->textureShaderProgram, "modelViewProjectionMatrix");

		glUseProgram(this->colourShaderProgram);
		this->mvpMatrixId_colourShader = glGetUniformLocation(this->colourShaderProgram, "modelViewProjectionMatrix");

		glUseProgram(this->textShaderProgram);
		this->orthoProjectionMatrixId = glGetUniformLocation(this->textShaderProgram, "projectionMatrix");


		glDisable(GL_CULL_FACE);

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
	}

	void Renderer::clearRenderList()
	{
		this->renderList.clear();
	}

	void Renderer::clearScreen(util::colour colour)
	{
		RenderCommand rc;
		rc.type = RenderCommand::CommandType::Clear;
		rc.colours.push_back(colour.toVec4());

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
		glViewport(0, /*(int) (this->_height * resscale)*/ 0, (int) (this->_width * resscale), (int) (this->_height * resscale));

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}




	void Renderer::renderColouredVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals)
	{
		RenderCommand rc;
		rc.type		= RenderCommand::CommandType::RenderColouredVerticesFilled;
		rc.vertices	= verts;
		rc.colours	= colours;
		rc.normals	= normals;
		rc.uvs		= { };

		rc.dimensions = 3;
		rc.isInScreenSpace = false;

		this->renderList.push_back(rc);
	}

	void Renderer::renderStringInNormalisedScreenSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos, TextAlignment align)
	{
		pos.x *= this->_width;
		pos.y *= this->_height;

		this->renderStringInScreenSpace(txt, font, size, pos);
	}

	void Renderer::renderStringInScreenSpace(std::string str, Rx::Font* font, float size, glm::vec2 pos, TextAlignment align)
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

					rc.dimensions = 2;
					rc.isInScreenSpace = true;
					rc.textureToBind = font->glTextureID;

					// fill it up.
					auto pos = glm::vec2(round(xPos), round(yPos));

					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y0), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y1), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y0), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y1), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x1, gpos.y0), 0, 1)));
					rc.vertices.push_back(glm::round(glm::vec4(pos + scale * glm::vec2(gpos.x0, gpos.y1), 0, 1)));

					rc.uvs = {
								glm::vec2(gpos.u0, gpos.v0),
								glm::vec2(gpos.u0, gpos.v1),
								glm::vec2(gpos.u1, gpos.v0),
								glm::vec2(gpos.u1, gpos.v1),
								glm::vec2(gpos.u1, gpos.v0),
								glm::vec2(gpos.u0, gpos.v1)
							};

					this->renderList.push_back(rc);
				}

				xPos += (rightAlign ? -1 : 1) * scale * gpos.xAdvance;
			}
		}
	}












	void Renderer::renderAll()
	{
		glClearColor(this->clearColour.fr, this->clearColour.fg, this->clearColour.fb, this->clearColour.fa);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double rscale = this->_resolutionScale;
		glViewport(0, 0, (int) (this->_width * rscale), (int) (this->_height * rscale));

		// this->renderList.clear();

		// static GLuint uvBuffer = -1;
		// static GLuint vertBuffer = -1;
		// static GLuint colourBuffer = -1;

		// if(uvBuffer == (GLuint) -1)		glGenBuffers(1, &uvBuffer);
		// if(vertBuffer == (GLuint) -1)	glGenBuffers(1, &vertBuffer);
		// if(colourBuffer == (GLuint) -1)	glGenBuffers(1, &colourBuffer);

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

				case CType::RenderColouredVerticesFilled:	// fallthrough
				case CType::RenderColouredVerticesWireframe: {

					GLuint vertBuffer = -1;
					GLuint colourBuffer = -1;
					glGenBuffers(1, &vertBuffer);
					glGenBuffers(1, &colourBuffer);

					glUseProgram(this->colourShaderProgram);

					glm::mat4 mvp = this->projectionMatrix * this->cameraMatrix * glm::mat4(1.0);
					glUniformMatrix4fv(this->mvpMatrixId_colourShader, 1, GL_FALSE, glm::value_ptr(mvp));


					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// attribute. No particular reason for 0, but must match the layout in the shader.
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// if we have colours:
					assert(rc.colours.size() > 0);
					{
						glEnableVertexAttribArray(1);

						glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.colours.size() * sizeof(glm::vec4), &rc.colours[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// attribute. No particular reason for 1, but must match the layout in the shader.
							4,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glDrawArrays(rc.type == CType::RenderColouredVerticesWireframe ? GL_LINES : GL_TRIANGLES, 0, rc.vertices.size());

					glDeleteBuffers(1, &vertBuffer);
					glDeleteBuffers(1, &colourBuffer);

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

				} break;





				case CType::RenderTexturedVerticesFilled:	// fallthrough
				case CType::RenderTexturedVerticesWireframe: {

					GLuint vertBuffer = -1;
					GLuint uvBuffer = -1;
					glGenBuffers(1, &vertBuffer);
					glGenBuffers(1, &uvBuffer);

					glUseProgram(this->textureShaderProgram);

					glm::mat4 mvp = this->projectionMatrix * this->cameraMatrix * glm::mat4(1.0);
					glUniformMatrix4fv(this->mvpMatrixId_textureShader, 1, GL_FALSE, glm::value_ptr(mvp));

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// attribute. No particular reason for 0, but must match the layout in the shader.
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					assert(rc.textureToBind != -1);
					{
						GL::pushTextureBinding(rc.textureToBind);
						assert(rc.uvs.size() > 0);

						glEnableVertexAttribArray(1);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// attribute. No particular reason for 1, but must match the layout in the shader.
							2,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glDrawArrays(rc.type == CType::RenderColouredVerticesWireframe ? GL_LINES : GL_TRIANGLES, 0, rc.vertices.size());

					glDeleteBuffers(1, &vertBuffer);
					glDeleteBuffers(1, &uvBuffer);

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

				} break;



				case CType::RenderText: {

					GLuint vertBuffer = -1;
					GLuint uvBuffer = -1;
					glGenBuffers(1, &vertBuffer);
					glGenBuffers(1, &uvBuffer);

					glUseProgram(this->textShaderProgram);

					glm::mat4 orthoProj = glm::ortho(0.0, this->_width, this->_height, 0.0);
					glUniformMatrix4fv(this->orthoProjectionMatrixId, 1, GL_FALSE, glm::value_ptr(orthoProj));

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							0,			// attribute. No particular reason for 0, but must match the layout in the shader.
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					// GL::pushTextureBinding(rc.textureToBind);

					glBindTexture(GL_TEXTURE_2D, rc.textureToBind);
					glEnableVertexAttribArray(1);
					{
						assert(rc.uvs.size() > 0);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0],
							GL_STATIC_DRAW);

						glVertexAttribPointer(
							1,			// attribute. No particular reason for 1, but must match the layout in the shader.
							2,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glDrawArrays(GL_TRIANGLES, 0, rc.vertices.size());

					glDeleteBuffers(1, &vertBuffer);
					glDeleteBuffers(1, &uvBuffer);

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

					// GL::popTextureBinding();

				} break;

				case CType::Invalid:
					ERROR("Invalid render command type");
			}
		}

		this->clearRenderList();
	}












































	// imgui stuff
	void PreFrame(Rx::Renderer* r)
	{
	}

	void BeginFrame(Rx::Renderer* renderer)
	{
		// ImGui_ImplSdl_NewFrame(renderer->window->sdlWin);
	}

	void EndFrame(Rx::Renderer* renderer)
	{
		renderer->renderAll();
		SDL_GL_SwapWindow(renderer->window->sdlWin);
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
	void Renderer::renderStringRightAligned(std::string txt, Rx::Font font, float size, Math::Vector2 pt)
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

	size_t Renderer::getStringWidthInPixels(std::string txt, Rx::Font font, float size)
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












