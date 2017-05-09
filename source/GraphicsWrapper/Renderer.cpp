// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "glwrapper.h"
#include "graphicswrapper.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"

using namespace Math;


namespace Rx
{
	void Renderer::Clear()
	{
		RenderCommand rc;
		rc.type = RenderCommand::CommandType::Clear;
		rc.colour = this->clearColour;

		this->renderList.push_back(rc);
	}

	void Renderer::RenderPoint(Math::Vector2 pt)
	{
		RenderCommand rc;
		rc.mode = GL_POINTS;
		rc.colour = this->drawColour;
		rc.type = RenderCommand::CommandType::RenderVerts;

		// need to convert to opengl coords
		// a rect where 0,0 is the centre, -1 and 1 are the sides.

		rc.vertices.push_back({ pt.x - 1.0, -1.0 * (pt.y - 1.0) });
		this->renderList.push_back(rc);
	}

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

	void Renderer::RenderRect(Math::Rectangle rect, bool fill)
	{
		RenderCommand rc;
		rc.mode = (fill ? GL_POLYGON : GL_LINE_LOOP);
		rc.colour = this->drawColour;
		rc.type = RenderCommand::CommandType::RenderVerts;


		double x1 = rect.origin.x;
		double y1 = rect.origin.y;

		double x2 = x1 + rect.width;
		double y2 = y1 + rect.height;


		rc.vertices.push_back({ x1, y1 });
		rc.vertices.push_back({ x2, y1 });
		rc.vertices.push_back({ x2, y2 });
		rc.vertices.push_back({ x1, y2 });

		this->renderList.push_back(rc);
	}

	void Renderer::RenderLine(Math::Vector2 start, Math::Vector2 end)
	{
		RenderCommand rc;
		rc.mode = GL_LINES;
		rc.colour = this->drawColour;
		rc.type = RenderCommand::CommandType::RenderVerts;

		rc.vertices.push_back({ start.x, start.y });
		rc.vertices.push_back({ end.x, end.y });

		this->renderList.push_back(rc);
	}













	void Renderer::RenderTex(Texture* tex, Math::Vector2 pt)
	{
		this->RenderTex(tex, pt.x, pt.y);
	}

	void Renderer::RenderTex(Texture* tex, uint32_t x, uint32_t y)
	{
		this->RenderTex(tex, Math::Rectangle(0, 0, tex->width, tex->height), Math::Rectangle(x, y, tex->width, tex->height));
	}

	void Renderer::RenderTex(Texture* tex, Math::Rectangle dest)
	{
		this->RenderTex(tex, dest.origin.x, dest.origin.y);
	}

	void Renderer::RenderTex(Texture* tex, Math::Rectangle src, Math::Rectangle dest)
	{
		this->renderList.push_back(RenderCommand::createRenderTexture(tex, src, dest));
	}

	void Renderer::RenderString(std::string txt, Rx::Font font, float size, Math::Vector2 pt)
	{
		this->renderList.push_back(RenderCommand::createRenderString(txt, font, size, this->drawColour, pt));
	}

	size_t Renderer::getStringWidthInPixels(std::string txt, Rx::Font font, float size)
	{
		if(txt.empty()) return 0;

		// note: this is basically going through each glyph twice, so only use when necessary.
		auto cmd = RenderCommand::createRenderString(txt, font, size, this->drawColour, Math::Vector2(0, 0));
		return (size_t) fabs(cmd.bounds.second.x - cmd.bounds.first.x);
	}

	void Renderer::SetColour(Util::Colour c)
	{
		this->drawColour = c;
	}


	void Renderer::RenderAll()
	{
		for(auto rc : this->renderList)
			rc.doRender();

		this->renderList.clear();
	}














































	// imgui stuff
	void PreFrame(Rx::Renderer* r)
	{
	}

	void BeginFrame(Rx::Renderer* renderer)
	{
		ImGui_ImplSdl_NewFrame(renderer->window->sdlWin);
	}

	void EndFrame(Rx::Renderer* renderer)
	{
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

			SetupOpenGL(dd, &fb_w, &fb_h);

			// idk if we need to change the order of drawing.
			// the way this is set up we can't interweave.

			// do ours first, as I get the feeling we want windows to be on top
			glDisable(GL_SCISSOR_TEST);
			renderer->RenderAll();


			// then render imgui
			RenderImGui(dd, fb_h);



			// finish
			FinishOpenGL();
		}

		SDL_GL_SwapWindow(renderer->window->sdlWin);
	}









	// split the RenderDrawLists function into separate parts.
	// part 1 sets up the opengl stuff
	// part 2 does the rendering of the imgui drawlist
	// part 3 renders *OUR* things
	// part 4 finishes the opengl stuff

	void SetupOpenGL(ImDrawData* draw_data, int* fb_width, int* fb_height)
	{
		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		// glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context


		// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
		ImGuiIO& io = ImGui::GetIO();
		*fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		*fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);


		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei) *fb_width, (GLsizei) *fb_height);

		// Setup orthographic projection matrix
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
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


	void FinishOpenGL()
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
}
























