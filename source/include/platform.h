// platform.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "input.h"

namespace rx
{
	struct Window;
	struct Renderer;
}

namespace platform
{
	// returns the window, as well as some user-defined data.
	std::pair<rx::Window*, void*> Initialise();

	// called at the end
	void Uninitialise(rx::Window* window, void* userdata);

	// initialises the actual platform. returns the userdata struct; don't call this from userspace.
	void* initialiseUserData();
	void uninitialiseUserData(void* userdata);

	// called after everything.
	void endProgram();

	// returns a platform-specific struct of a window (not the same as userdata)
	void* createWindow(void* userdata, std::string title, int width, int height, bool allowResize);
	void destroyWindow(void* userdata, void* userwindow);
	int getWindowScale(void* userdata, void* userwindow);
	glm::vec2 getMousePosition(void* userdata);

	void updateInput(input::State* inputState, double delta, void* userdata);

	bool hasEvent(void* userdata);
	std::vector<input::Event> getEvents(void* userdata);

	// returns true if it's time to quit
	bool processEvent(input::Event event, void* userdata);

	void preFrame(void* userdata, void* userwindow);
	void beginFrame(void* userdata, void* userwindow);

	void endFrame(void* userdata, void* userwindow);
}
















