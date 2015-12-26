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
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::RenderPoint(Math::Vector2 pt)
	{
		glBegin(GL_POINTS);
		this->updateGlColour();

		glVertex2d(pt.x, pt.y);

		glEnd();
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
		if(fill)	glBegin(GL_POLYGON);
		else		glBegin(GL_LINE_LOOP);

		glColor4ub(this->drawColour.r, this->drawColour.g, this->drawColour.b, this->drawColour.a);

		glVertex2d(rect.origin.x, rect.origin.y);
		glVertex2d(rect.origin.x + rect.width, rect.origin.y);
		glVertex2d(rect.origin.x + rect.width, rect.origin.y + rect.height);
		glVertex2d(rect.origin.x, rect.origin.y + rect.height);

		glEnd();
	}

	void Renderer::RenderLine(Math::Vector2 start, Math::Vector2 end)
	{
		glBegin(GL_LINES);
		this->updateGlColour();

		glVertex2d(start.x, start.y);
		glVertex2d(end.x, end.y);

		glEnd();
	}

	void Renderer::RenderEqTriangle(Math::Vector2 centre, double side)
	{
		glBegin(GL_TRIANGLES);
		this->updateGlColour();

		// left edge (x1, y1)
		// top edge (x2, y2)
		// right edge (x3, y3)

		// height = sqrt((hyp^2) - ((side / 2)^2))
		double halfSide = side / 2;
		double height = sqrt((side * side) - (halfSide * halfSide));


		// x1 = origin.x - (side / 2), y1 = origin.y + (1/3 * height)
		// x3 = origin.x + (side / 2), y3 = origin.y + (1/3 * height)
		// x2 = origin.x, y2 = origin.y - (2/3 * height)

		Math::Vector2 left(centre.x - halfSide, centre.y + ((1.0 / 3.0) * height));
		Math::Vector2 right(centre.x + halfSide, centre.y + ((1.0 / 3.0) * height));
		Math::Vector2 top(centre.x, centre.y - ((2.0 / 3.0) * height));

		glVertex2d(left.x, left.y);
		glVertex2d(top.x, top.y);
		glVertex2d(right.x, right.y);

		glEnd();
	}












	void Renderer::Render(Texture* tex, Math::Vector2 pt)
	{
		this->Render(tex, pt.x, pt.y);
	}

	void Renderer::Render(Texture* tex, uint32_t x, uint32_t y)
	{
		this->Render(tex, Math::Rectangle(0, 0, tex->width, tex->height), Math::Rectangle(x, y, tex->width, tex->height));
	}

	void Renderer::Render(Texture* tex, Math::Rectangle dest)
	{
		this->Render(tex, dest.origin.x, dest.origin.y);
	}

	void Renderer::Render(Texture* tex, Math::Rectangle src, Math::Rectangle dest)
	{
		double x = dest.origin.x;
		double y = dest.origin.y;

		double w = dest.width;
		double h = dest.height;

		double texOffsetX1 = (double) src.origin.x / (double) tex->width;
		double texOffsetY1 = (double) src.origin.y / (double) tex->height;
		double texOffsetX2 = (double) ((src.origin.x + src.width) / (double) tex->width);
		double texOffsetY2 = (double) ((src.origin.y + src.height) / (double) tex->height);


		glBindTexture(GL_TEXTURE_2D, tex->glTextureID);
		this->updateGlColour();
		glBegin(GL_QUADS);
		{
			glTexCoord2d(texOffsetX1, texOffsetY1);		glVertex3d(x, y, 0);
			glTexCoord2d(texOffsetX2, texOffsetY1);		glVertex3d(x + w, y, 0);
			glTexCoord2d(texOffsetX2, texOffsetY2);		glVertex3d(x + w, y + h, 0);
			glTexCoord2d(texOffsetX1, texOffsetY2);		glVertex3d(x, y + h, 0);
		}
		glEnd();
	}

	void Renderer::RenderText(std::string txt, ImFont* font, Math::Vector2 pt)
	{
		// auto dl = ImGui::GetWindowDrawList();
		// font->RenderText(font->FontSize, { (float) pt.x, (float) pt.y }, Util::Colour::white().hex(), dl->_ClipRectStack.back(),
		// 	txt.c_str(), txt.c_str() + txt.length(), dl);
	}

	void Renderer::SetColour(Util::Colour c)
	{
		// SDL_SetRenderDrawBlendMode(this->sdlRenderer, SDL_BLENDMODE_BLEND);
		// SDL_SetRenderDrawColor(this->sdlRenderer, c.r, c.g, c.b, c.a);
		this->drawColour = c;
	}

	void Renderer::updateGlColour()
	{
		glColor4ub(this->drawColour.r, this->drawColour.g, this->drawColour.b, this->drawColour.a);
	}
}



