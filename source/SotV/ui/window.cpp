// window.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>

#include "sotv/sotv.h"
#include "sotv/gui.h"

namespace Sotv
{
	static size_t curId = 1;

	Window::Window()
	{
		this->isWindowOpen = true;
		this->id = curId++;
	}

	Window::~Window()
	{
	}

	void Window::Close(GameState& gs)
	{
		assert(!this->isWindowOpen && "cannot close open window");

		// remove us from the windowlist.
		auto it = std::find(gs.windowList.begin(), gs.windowList.end(), this);
		assert(it != gs.windowList.end() && "window not in list");

		// TODO(synchro): lock this maybe
		gs.windowList.erase(it);


		delete this;
	}

	bool Window::isOpen()
	{
		return this->isWindowOpen;
	}

	void Window::Render(GameState& gs, float delta, Rx::Renderer* r)
	{
	}

	void Window::Update(GameState& gs, float delta)
	{
	}
}
