// imguiwrapper.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include "sdlwrapper.h"

namespace IG
{
	std::pair<SDL_GLContext, SDL::Renderer*> Initialise(int width, int height, Util::Colour clear);
	std::pair<SDL_Event, bool> ProcessEvents();

	// rendering stuff.
	void BeginFrame(SDL::Renderer* w);
	void EndFrame(SDL::Renderer* w);
}
