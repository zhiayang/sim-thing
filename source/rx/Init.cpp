// Init.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>
#include <assert.h>

#include "rx.h"
#include "config.h"
#include "platform.h"

#include <glbinding/Binding.h>

namespace platform
{
	std::pair<rx::Window*, void*> Initialise()
	{
		std::string windowTitle = "SotV";

		void* userdata = platform::initialiseUserData();
		void* userwindow = platform::createWindow(userdata, windowTitle, Config::getResX(), Config::getResY(), true);

		int scale = platform::getWindowScale(userdata, userwindow);
		rx::Window* window = new rx::Window(windowTitle, userdata, userwindow, Config::getResX(), Config::getResY(), scale);

		glbinding::Binding::initialize();

		return { window, userdata };
	}

	void Uninitialise(rx::Window* window, void* userdata)
	{
		assert(window->platformData == userdata);
		platform::destroyWindow(userdata, window->platformWindow);

		platform::uninitialiseUserData(userdata);
	}
}


namespace rx
{
	Window::~Window()
	{
		platform::destroyWindow(this->platformData, this->platformWindow);
	}
}






























