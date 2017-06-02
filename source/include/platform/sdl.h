// sdl.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include <SDL2/SDL.h>

namespace platform
{
	struct SDLUserData
	{
		int windowWidth = 0;
		int windowHeight = 0;

		SDL_GLContext glContext = 0;
	};
}
