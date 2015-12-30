// Init.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>

#include "imgui.h"

#include "graphicswrapper.h"
#include "imgui_impl_sdl.h"

namespace Rx
{
	std::pair<SDL_GLContext, Rx::Renderer*> Initialise(int width, int height, Util::Colour clear)
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





		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = { (float) width, (float) height };

		// Setup window
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

		Rx::Window* window = new Rx::Window("connect", io.DisplaySize.x, io.DisplaySize.y, true);
		SDL_GLContext glcontext = SDL_GL_CreateContext(window->sdlWin);

		// Setup ImGui binding
		ImGui_ImplSdl_Init(window->sdlWin);

		Rx::Renderer* renderer = new Rx::Renderer(window, glcontext, clear);

		return { glcontext, renderer };
	}


	std::pair<SDL_Event, bool> ProcessEvents()
	{
		bool done = false;

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			ImGui_ImplSdl_ProcessEvent(&event);
			if(event.type == SDL_QUIT)
				done = true;
		}

		return { event, done };
	}
}






























