// Init.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>

#include "config.h"
#include "renderer/rx.h"

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

namespace Rx
{
	std::pair<SDL_GLContext, Rx::Window*> Initialise(int width, int height)
	{
		// initialise SDL
		{
			auto err = SDL_Init(SDL_INIT_EVERYTHING);
			if(err)	ERROR("SDL failed to initialise, subsystem flags: %d", SDL_INIT_EVERYTHING);

			// init SDL_Image
			if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
				ERROR("Failed to initialise SDL2_image library");

			TTF_Init();
		}





		// ImGuiIO& io = ImGui::GetIO();
		// io.DisplaySize = { (float) width, (float) height };

		// Setup window
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_CaptureMouse((SDL_bool) true);
		SDL_GL_SetSwapInterval(1);

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);


		Rx::Window* window = new Rx::Window("connect", Config::getResX(), Config::getResY(), true);
		SDL_GLContext glcontext = SDL_GL_CreateContext(window->sdlWin);
		if(glcontext == 0)
			ERROR("Failed to create OpenGL context. SDL Error: '%s'", SDL_GetError());

		glbinding::Binding::initialize();

		// Setup ImGui binding
		// ImGui_ImplSdl_Init(window->sdlWin);

		return { glcontext, window };
	}
}






























