// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "glwrapper.h"
#include "graphicswrapper.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"

using namespace Math;

// copied from imgui.cpp
static int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);



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

		rc.vertices.push_back({ rect.origin.x - 1.0, -1.0 * (rect.origin.y - 1.0) });
		rc.vertices.push_back({ rect.origin.x - 1.0 + rect.width * 2.0, -1.0 * (rect.origin.y - 1.0) });
		rc.vertices.push_back({ rect.origin.x - 1.0 + rect.width * 2.0, -1.0 * (rect.origin.y - 1.0 + rect.height * 2.0) });
		rc.vertices.push_back({ rect.origin.x - 1.0, -1.0 * (rect.origin.y - 1.0 + rect.height * 2.0) });

		this->renderList.push_back(rc);
	}

	void Renderer::RenderLine(Math::Vector2 start, Math::Vector2 end)
	{
		RenderCommand rc;
		rc.mode = GL_LINES;
		rc.colour = this->drawColour;
		rc.type = RenderCommand::CommandType::RenderVerts;

		rc.vertices.push_back({ start.x - 1.0, -1.0 * (start.y - 1.0) });
		rc.vertices.push_back({ end.x - 1.0, -1.0 * (end.y - 1.0) });

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











	void RenderCommand::doRender()
	{
		if(this->type == CommandType::Clear)
		{
			glClearColor(this->colour.fr, this->colour.fg, this->colour.fb, this->colour.fa);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		else if(this->type == CommandType::RenderVerts)
		{
			glBegin(this->mode);
			glColor4f(this->colour.fr, this->colour.fg, this->colour.fb, this->colour.fa);

			for(auto v : this->vertices)
				glVertex2d(v.x, v.y);

			glEnd();
		}
		else if(this->type == CommandType::RenderQuads)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, this->textureId);
			glColor4f(this->colour.fr, this->colour.fg, this->colour.fb, this->colour.fa);


			for(size_t i = 0; i < this->vertices.size(); i += 4)
			{
				float x1 = this->vertices[i + 0].x;
				float y1 = this->vertices[i + 0].y;

				float x2 = this->vertices[i + 1].x;
				float y2 = this->vertices[i + 1].y;

				float u1 = this->vertices[i + 2].x;
				float v1 = this->vertices[i + 2].y;

				float u2 = this->vertices[i + 3].x;
				float v2 = this->vertices[i + 3].y;

				glBegin(GL_QUADS);
				{
					glTexCoord2f(u1, v1);		glVertex3f(x1, y1, 0);
					glTexCoord2f(u2, v1);		glVertex3f(x2, y1, 0);
					glTexCoord2f(u2, v2);		glVertex3f(x2, y2, 0);
					glTexCoord2f(u1, v2);		glVertex3f(x1, y2, 0);
				}
				glEnd();
			}
		}
	}

	RenderCommand RenderCommand::createRenderTexture(Texture* tex, Math::Rectangle src, Math::Rectangle dest)
	{
		// we setup four vertices
		double x1 = dest.origin.x;
		double y1 = dest.origin.y;

		double x2 = dest.origin.x + dest.width;
		double y2 = dest.origin.y + dest.height;

		double texOffsetX1 = (double) src.origin.x / (double) tex->width;
		double texOffsetY1 = (double) src.origin.y / (double) tex->height;

		double texOffsetX2 = (double) (src.origin.x + src.width) / (double) tex->width;
		double texOffsetY2 = (double) (src.origin.y + src.height) / (double) tex->height;


		Math::Vector2 v1 { x1, y1 };
		Math::Vector2 v2 { x2, y2 };
		Math::Vector2 v3 { texOffsetX1, texOffsetY1 };
		Math::Vector2 v4 { texOffsetX2, texOffsetY2 };

		RenderCommand rc;
		rc.type = CommandType::RenderQuads;
		rc.colour = Util::Colour::white();

		rc.vertices.push_back(v1);
		rc.vertices.push_back(v2);
		rc.vertices.push_back(v3);
		rc.vertices.push_back(v4);

		rc.textureId = tex->glTextureID;

		return rc;
	}

	RenderCommand RenderCommand::createRenderVertices(std::vector<Math::Vector2> vertices, GLenum mode, Util::Colour col, bool fill)
	{
		RenderCommand rc;
		rc.type = CommandType::RenderVerts;

		rc.colour = col;
		rc.vertices = vertices;
		rc.fill = fill;
		rc.mode = mode;

		return rc;
	}












	RenderCommand RenderCommand::createRenderString(std::string str, Rx::Font font, float size, Util::Colour col, Math::Vector2 pos)
	{
		RenderCommand rc;
		rc.type = CommandType::RenderQuads;
		rc.colour = col;

		// prevent blurry shit

		// todo:
		// because imgui is poorly designed, if the font hasn't been loaded, we can't switch to it.
		// we can't load fonts in the middle of a frame, because its font atlas is stupid.
		// so, if the desired size font hasn't been loaded, we can only deal with.. shitty scaled up stuff.
		// see workaround in main.cpp.

		rc.textureId = (int) (uintptr_t) font.imgui->ContainerAtlas->TexID;


		{
			ImVec4 cliprect;
			ImFont& fnt = *font.imgui;
			{
				const float scale = (size / fnt.FontSize) * (1.0 / ImGui::GetIO().FontGlobalScale);

				float x1 = pos.x;
				float y1 = pos.y;

				float advx = 0;
				for(auto c : str)
					advx += scale * font.imgui->GetCharAdvance(c);

				float advy = scale * -font.imgui->Descent;

				float x2 = x1 + advx;
				float y2 = y1 + advy;

				cliprect = { x1, y1, x2, y2 };
			}




			// Align to be pixel perfect
			pos.x = (float)(int)(pos.x + fnt.DisplayOffset.x);
			pos.y = (float)(int)(pos.y + fnt.DisplayOffset.y);
			float x = pos.x;
			float y = pos.y;

			assert(y <= cliprect.w);

			const float scale = size / fnt.FontSize;
			const float line_height = fnt.FontSize * scale;
			const float wrap_width = 0;
			const bool word_wrap_enabled = wrap_width > 0.0f;
			const char* word_wrap_eol = NULL;

			const char* s = str.c_str();
			const char* text_end = str.c_str() + str.length();

			if(!word_wrap_enabled && y + line_height < cliprect.y)
			{
				while(s < text_end && *s != '\n')  // Fast-forward to next line
					s++;
			}

			while(s < text_end)
			{
				if(word_wrap_enabled)
				{
					// Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
					if(!word_wrap_eol)
					{
						word_wrap_eol = fnt.CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
						if(word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
							word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
					}

					if(s >= word_wrap_eol)
					{
						x = pos.x;
						y += line_height;
						word_wrap_eol = NULL;

						// Wrapping skips upcoming blanks
						while(s < text_end)
						{
							const char c = *s;
							auto is_space = [](int c) -> bool { return c == ' ' || c == '\t' || c == 0x3000; };

							if(is_space(c)) { s++; } else if(c == '\n') { s++; break; } else { break; }
						}
						continue;
					}
				}

				// Decode and advance source
				unsigned int c = (unsigned int)*s;
				if(c < 0x80)
				{
					s += 1;
				}
				else
				{
					s += ImTextCharFromUtf8(&c, s, text_end);
					if(c == 0)
						break;
				}

				if(c < 32)
				{
					if(c == '\n')
					{
						x = pos.x;
						y += line_height;

						if(y > cliprect.w)
							break;
						if(!word_wrap_enabled && y + line_height < cliprect.y)
							while(s < text_end && *s != '\n')  // Fast-forward to next line
								s++;
						continue;
					}
					if(c == '\r')
						continue;
				}

				float char_width = 0.0f;
				if(const ImFont::Glyph* glyph = fnt.FindGlyph((unsigned short)c))
				{
					char_width = glyph->XAdvance * scale;

					// Clipping on Y is more likely
					if(c != ' ' && c != '\t')
					{
						// We don't do a second finer clipping test on the Y axis (TODO: do some measurement see if it is worth it, probably not)
						float y1 = (float)(y + glyph->Y0 * scale);
						float y2 = (float)(y + glyph->Y1 * scale);

						float x1 = (float)(x + glyph->X0 * scale);
						float x2 = (float)(x + glyph->X1 * scale);
						if(x1 <= cliprect.z && x2 >= cliprect.x)
						{
							// Render a character
							float u1 = glyph->U0;
							float v1 = glyph->V0;
							float u2 = glyph->U1;
							float v2 = glyph->V1;

							// CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
							if(false)
							{
								if(x1 < cliprect.x)
								{
									u1 = u1 + (1.0f - (x2 - cliprect.x) / (x2 - x1)) * (u2 - u1);
									x1 = cliprect.x;
								}
								if(y1 < cliprect.y)
								{
									v1 = v1 + (1.0f - (y2 - cliprect.y) / (y2 - y1)) * (v2 - v1);
									y1 = cliprect.y;
								}
								if(x2 > cliprect.z)
								{
									u2 = u1 + ((cliprect.z - x1) / (x2 - x1)) * (u2 - u1);
									x2 = cliprect.z;
								}
								if(y2 > cliprect.w)
								{
									v2 = v1 + ((cliprect.w - y1) / (y2 - y1)) * (v2 - v1);
									y2 = cliprect.w;
								}
								if(y1 >= y2)
								{
									x += char_width;
									continue;
								}
							}

							// NB: we are not calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
							// inlined:
							{
								rc.vertices.push_back({ x1, y1 });
								rc.vertices.push_back({ x2, y2 });

								rc.vertices.push_back({ u1, v1 });
								rc.vertices.push_back({ u2, v2 });
							}
						}
					}
				}

				x += char_width;
			}
		}

		return rc;
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
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int) pcmd->ClipRect.x, (int) (fb_height - pcmd->ClipRect.w),
						(int) (pcmd->ClipRect.z - pcmd->ClipRect.x), (int) (pcmd->ClipRect.w - pcmd->ClipRect.y));

					glDrawElements(GL_TRIANGLES, (GLsizei) pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);
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

		glBindTexture(GL_TEXTURE_2D, 0);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}
}






















// copied from imgui.cpp

// Convert UTF-8 to 32-bits character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
static int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
	unsigned int c = (unsigned int)-1;
	const unsigned char* str = (const unsigned char*)in_text;
	if(!(*str & 0x80))
	{
		c = (unsigned int)(*str++);
		*out_char = c;
		return 1;
	}
	if((*str & 0xe0) == 0xc0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if(in_text_end && in_text_end - (const char*)str < 2) return 1;
		if(*str < 0xc2) return 2;
		c = (unsigned int)((*str++ & 0x1f) << 6);
		if((*str & 0xc0) != 0x80) return 2;
		c += (*str++ & 0x3f);
		*out_char = c;
		return 2;
	}
	if((*str & 0xf0) == 0xe0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if(in_text_end && in_text_end - (const char*)str < 3) return 1;
		if(*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
		if(*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x0f) << 12);
		if((*str & 0xc0) != 0x80) return 3;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if((*str & 0xc0) != 0x80) return 3;
		c += (*str++ & 0x3f);
		*out_char = c;
		return 3;
	}
	if((*str & 0xf8) == 0xf0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if(in_text_end && in_text_end - (const char*)str < 4) return 1;
		if(*str > 0xf4) return 4;
		if(*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
		if(*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x07) << 18);
		if((*str & 0xc0) != 0x80) return 4;
		c += (unsigned int)((*str++ & 0x3f) << 12);
		if((*str & 0xc0) != 0x80) return 4;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if((*str & 0xc0) != 0x80) return 4;
		c += (*str++ & 0x3f);
		// utf-8 encodings of values used in surrogate pairs are invalid
		if((c & 0xFFFFF800) == 0xD800) return 4;
		*out_char = c;
		return 4;
	}
	*out_char = 0;
	return 0;
}




