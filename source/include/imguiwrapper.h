// imguiwrapper.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include "sdlwrapper.h"

struct ImDrawData;

namespace IG
{
	std::pair<SDL_GLContext, SDL::Renderer*> Initialise(int width, int height, Util::Colour clear);
	std::pair<SDL_Event, bool> ProcessEvents();

	// rendering stuff.
	void BeginFrame(SDL::Renderer* w);
	void EndFrame(SDL::Renderer* w);




	// internal stuff, mostly
	void SetupOpenGL(ImDrawData* draw_data, int* fb_width, int* fb_height);
	void RenderImGui(ImDrawData* draw_data, int fb_height);
	void FinishOpenGL();
}
