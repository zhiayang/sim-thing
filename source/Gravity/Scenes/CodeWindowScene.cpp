// Scene.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "gravity/gravity.h"
#include "graphicswrapper.h"

namespace Gravity
{
	void CodeWindowScene::renderScene(Controller& c, Rx::Renderer* r)
	{
		// draw a box.
		r->SetColour(Util::Colour::white());
		r->RenderRect(Math::Rectangle(this->xpos + 1, this->ypos + 1, this->width - 1, this->height - 1), false);
	}

	void CodeWindowScene::updateScene(Controller& c, double delta)
	{
	}

	bool CodeWindowScene::hidesBehind()
	{
		return false;
	}



	CodeWindowScene::CodeWindowScene(size_t x, size_t y, size_t w, size_t h)
	{
		this->xpos = x;
		this->ypos = y;

		this->width = w;
		this->height = h;
	}
}
