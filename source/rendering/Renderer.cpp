// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "glwrapper.h"
#include "graphicswrapper.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"

#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace gl;
using namespace Math;


namespace Rx
{
	Renderer::Renderer(Window* win, SDL_GLContext glc, util::colour clearCol, glm::mat4 camera, gl::GLuint mainShaderProg,
			gl::GLuint textShaderProg, double fov, double width, double height, double near, double far)
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

		this->mainShaderProgram = mainShaderProg;
		this->textShaderProgram = textShaderProg;

		this->mvpMatrixId = glGetUniformLocation(this->mainShaderProgram, "modelViewProjectionMatrix");

		// bind permanently...?
		gl::GLuint vertexArrayID;
		gl::glGenVertexArrays(1, &vertexArrayID);
		gl::glBindVertexArray(vertexArrayID);

		glViewport(0, /*(int) height*/ 0, (int) width, (int) height);
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

		// redo the projection matrix
		this->projectionMatrix = glm::perspective(this->_fov, this->_width / this->_height, this->_near, this->_far);
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

	void Renderer::renderVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals,
		std::vector<glm::vec2> uvs)
	{
		RenderCommand rc;
		rc.type		= RenderCommand::CommandType::RenderVerticesFilled;
		rc.vertices	= verts;
		rc.colours	= colours;
		rc.normals	= normals;
		rc.uvs		= uvs;

		rc.dimensions = 3;
		rc.isInScreenSpace = false;

		this->renderList.push_back(rc);
	}

	void Renderer::renderStringInScreenSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos)
	{
		auto gpos = getGlyphPosition(font, 'F');
		RenderCommand rc;

		glm::vec2 divisor = glm::vec2(this->_width, this->_height);

		// convert to clipspace
		glm::vec2 clipspace = pos / divisor;
		printf("clipspace = (%f, %f)\n", clipspace.x, clipspace.y);

		for(auto& v : gpos.vertices)
		{
			printf("(%f, %f)", v.x, v.y);
			v /= divisor;
			printf("[%f, %f]\n", v.x, v.y);
		}

		rc.type = RenderCommand::CommandType::RenderText;

		rc.dimensions = 2;
		rc.isInScreenSpace = true;

		// glm::vec4 view = glm::vec4(0, 0, this->_width, this->_height);

		auto invMVP = glm::inverse(this->projectionMatrix * this->cameraMatrix);

		glm::vec2 x0y0 = clipspace + (gpos.vertices[0] / divisor);
		glm::vec2 x1y0 = clipspace + (gpos.vertices[1] / divisor);
		glm::vec2 x1y1 = clipspace + (gpos.vertices[2] / divisor);
		glm::vec2 x0y1 = clipspace + (gpos.vertices[3] / divisor);


		rc.vertices.push_back(glm::vec4(x0y0, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(x0y1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(x1y0, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec3(glm::vec4(x1y1, 0, 1) * invMVP));
		rc.vertices.push_back(glm::vec3(glm::vec4(x1y0, 0, 1) * invMVP));
		rc.vertices.push_back(glm::vec3(glm::vec4(x0y1, 0, 1) * invMVP));

		rc.uvs = {
					gpos.uvs[0],
					gpos.uvs[3],
					gpos.uvs[1],
					gpos.uvs[2],
					gpos.uvs[1],
					gpos.uvs[3]
				};

		// setup the vertices
/*
		rc.vertices.push_back(glm::vec4(-1, -1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(-1, 1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(1, 1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(-1, -1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(1, 1, 0, 1) * invMVP);
		rc.vertices.push_back(glm::vec4(1, -1, 0, 1) * invMVP);
*/
		rc.textureToBind = font->glTextureID;
		this->renderList.push_back(rc);
	}












	void Renderer::renderAll()
	{
		gl::glEnable(gl::GL_DEPTH_TEST);
		gl::glDepthFunc(gl::GL_LESS);

		glClearColor(this->clearColour.fr, this->clearColour.fg, this->clearColour.fb, this->clearColour.fa);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 mvp = this->projectionMatrix * this->cameraMatrix * glm::mat4(1.0);
		glUniformMatrix4fv(this->mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

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
				}
				break;

				case CType::RenderVerticesFilled:	// fallthrough
				case CType::RenderVerticesWireframe: {

					glUseProgram(this->mainShaderProgram);

					GLuint uvBuffer = -1;
					GLuint vertBuffer = -1;
					GLuint colourBuffer = -1;

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glGenBuffers(1, &vertBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0].x,
							GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);

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
					if(rc.colours.size() > 0)
					{
						glEnableVertexAttribArray(1);

						glGenBuffers(1, &colourBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.colours.size() * sizeof(glm::vec4), &rc.colours[0].x,
							GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);

						glVertexAttribPointer(
							1,			// attribute. No particular reason for 1, but must match the layout in the shader.
							4,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}
					else if(rc.textureToBind != -1)
					{
						GL::pushTextureBinding(rc.textureToBind);
						assert(rc.uvs.size() > 0);

						glEnableVertexAttribArray(1);

						glGenBuffers(1, &uvBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0].x,
							GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

						glVertexAttribPointer(
							1,			// attribute. No particular reason for 1, but must match the layout in the shader.
							2,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glDrawArrays(rc.type == CType::RenderVerticesWireframe ? GL_LINES : GL_TRIANGLES, 0, rc.vertices.size());

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

					glDeleteBuffers(1, &uvBuffer);
					glDeleteBuffers(1, &vertBuffer);
					glDeleteBuffers(1, &colourBuffer);

					if(rc.textureToBind != -1)
						GL::popTextureBinding();
				}
				break;

				case CType::RenderText: {
					glUseProgram(this->textShaderProgram);

					GLuint uvBuffer = -1;
					GLuint vertBuffer = -1;

					glEnableVertexAttribArray(0);
					{
						// we always have vertices
						glGenBuffers(1, &vertBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.vertices.size() * sizeof(glm::vec3), &rc.vertices[0].x,
							GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);

						glVertexAttribPointer(
							0,			// attribute. No particular reason for 0, but must match the layout in the shader.
							3,			// size
							GL_FLOAT,	// type
							GL_FALSE,	// normalized?
							0,			// stride
							(void*) 0	// array buffer offset
						);
					}

					glEnableVertexAttribArray(1);
					{
						GL::pushTextureBinding(rc.textureToBind);
						assert(rc.uvs.size() > 0);

						glGenBuffers(1, &uvBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
						glBufferData(GL_ARRAY_BUFFER, rc.uvs.size() * sizeof(glm::vec2), &rc.uvs[0].x,
							GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

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

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);

					glDeleteBuffers(1, &uvBuffer);
					glDeleteBuffers(1, &vertBuffer);

					GL::popTextureBinding();
				}
				break;

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
		#if 0
		// call back to opengl
		glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);
		glClearColor(renderer->clearColour.fr, renderer->clearColour.fg, renderer->clearColour.fb, renderer->clearColour.fa);
		glClear(GL_COLOR_BUFFER_BIT);

		// this doesn't draw anything to the screen, since our "renderdrawlistfn" is 0
		ImGui::Render();



		// do it ourselves, here.
		{
			ImDrawData* dd = ImGui::GetDrawData();

			int fb_w = 0;
			int fb_h = 0;

			SetupOpenGL2D(dd, &fb_w, &fb_h);

			// idk if we need to change the order of drawing.
			// the way this is set up we can't interweave.

			// do ours first, as I get the feeling we want windows to be on top
			glDisable(GL_SCISSOR_TEST);

			// renderer->RenderAll();


			// then render imgui
			RenderImGui(dd, fb_h);



			// finish
			FinishOpenGL2D();
		}
		#endif

		// glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);

		renderer->renderAll();




















		// swap.
		SDL_GL_SwapWindow(renderer->window->sdlWin);
	}









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
}
























