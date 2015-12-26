// Renderer.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "glwrapper.h"
#include "sdlwrapper.h"
#include "imguiwrapper.h"

#include "imgui.h"

using namespace Math;

namespace SDL
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


		// if(fill)	glBegin(GL_POLYGON);
		// else		glBegin(GL_LINE_LOOP);

		// glColor4ub(this->drawColour.r, this->drawColour.g, this->drawColour.b, this->drawColour.a);

		// glVertex2d(rect.origin.x, rect.origin.y);
		// glVertex2d(rect.origin.x + rect.width, rect.origin.y);
		// glVertex2d(rect.origin.x + rect.width, rect.origin.y + rect.height);
		// glVertex2d(rect.origin.x, rect.origin.y + rect.height);

		// glEnd();

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

	void Renderer::RenderString(std::string txt, Util::Font font, Math::Vector2 pt)
	{
		this->renderList.push_back(RenderCommand::createRenderString(txt, font, this->drawColour, pt));
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
		else if(this->type == CommandType::RenderTex)
		{
			glBindTexture(GL_TEXTURE_2D, this->texture->glTextureID);
			glColor4f(this->colour.fr, this->colour.fg, this->colour.fb, this->colour.fa);


			float x1 = this->vertices[0].x;
			float y1 = this->vertices[0].y;

			float x2 = this->vertices[1].x;
			float y2 = this->vertices[1].y;

			float texOffsetX1 = this->vertices[2].x;
			float texOffsetY1 = this->vertices[2].y;

			float texOffsetX2 = this->vertices[3].x;
			float texOffsetY2 = this->vertices[3].y;

			glBegin(GL_QUADS);
			{
				printf("coords: (%f, %f) // (%f, %f), tex: (%f, %f) // (%f, %f)\n", x1, y1, x2, y2,
					texOffsetX1, texOffsetY1, texOffsetX2, texOffsetY2);

				glTexCoord2f(texOffsetX1, texOffsetY1);		glVertex3f(x1, y1, 0);
				glTexCoord2f(texOffsetX2, texOffsetY1);		glVertex3f(x2, y1, 0);
				glTexCoord2f(texOffsetX2, texOffsetY2);		glVertex3f(x2, y2, 0);
				glTexCoord2f(texOffsetX1, texOffsetY2);		glVertex3f(x1, y2, 0);
			}
			glEnd();
		}
		else if(this->type == CommandType::RenderString)
		{
			SDL::Surface* surface = SDL::Surface::fromText(this->font, this->colour, this->str);
			SDL::Texture* tex = new SDL::Texture(surface, 0);

			double x = this->vertices[0].x;
			double y = this->vertices[0].y;

			// auto rc = createRenderTexture(tex, Math::Rectangle(0, 0, 1, 1),
			// 	Math::Rectangle(x - 1.0, -1.0 * (y - 1.0), x - 1.0 + 2.0 * tex->width, -1 * (y - 1.0 + 2.0 * tex->height)));

			auto rc = createRenderTexture(tex, Math::Rectangle(0, 0, 1, 1),
				Math::Rectangle(x - 1.0, -1.0 * (y - 1.0), 1.0, 1.0));

			rc.doRender();
		}
	}

	RenderCommand RenderCommand::createRenderTexture(Texture* tex, Math::Rectangle src, Math::Rectangle dest)
	{
		// we setup four vertices
		double x1 = dest.origin.x - 1.0;
		double y1 = -1.0 * (dest.origin.y - 1.0);

		double w = dest.width / (double) tex->width;
		double h = dest.height / (double) tex->height;

		double x2 = dest.origin.x - 1.0 + 2.0 * w;
		double y2 = -1.0 * (dest.origin.y - 1.0 + 2.0 * h);

		double texOffsetX1 = (double) src.origin.x / (double) tex->width;
		double texOffsetY1 = (double) src.origin.y / (double) tex->height;

		double texOffsetX2 = (double) (src.origin.x + src.width) / (double) tex->width;
		double texOffsetY2 = (double) (src.origin.y + src.height) / (double) tex->height;

		Math::Vector2 v1 { x1, y1 };
		Math::Vector2 v2 { x2, y2 };
		Math::Vector2 v3 { texOffsetX1, texOffsetY1 };
		Math::Vector2 v4 { texOffsetX2, texOffsetY2 };

		RenderCommand rc;
		rc.type = CommandType::RenderTex;

		rc.vertices.push_back(v1);
		rc.vertices.push_back(v2);
		rc.vertices.push_back(v3);
		rc.vertices.push_back(v4);

		rc.texture = tex;

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

	RenderCommand RenderCommand::createRenderString(std::string s, Util::Font font, Util::Colour col, Math::Vector2 pos)
	{
		RenderCommand rc;
		rc.type = CommandType::RenderString;

		rc.colour = col;
		rc.vertices = { pos };
		rc.str = s;
		rc.font = font;

		return rc;
	}
}


























