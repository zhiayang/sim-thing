// Functions.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"

#include "utilities.h"
#include "glwrapper.h"
#include "sdlwrapper.h"

namespace SDL
{
	void Initialise()
	{
		SDL::Initialise(SDL_INIT_EVERYTHING);
	}

	void Initialise(uint32_t subs)
	{
		auto err = SDL_Init(subs);
		if(err)	ERROR("SDL failed to initialise, subsystem flags: %d", subs);

		// init SDL_Image
		if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
			ERROR("Failed to initialise SDL2_image library");
	}
}





































