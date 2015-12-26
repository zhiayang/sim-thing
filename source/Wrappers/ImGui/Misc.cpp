// Misc.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>

#include <imgui.h>

#include "imguiwrapper.h"
#include "imgui_impl_sdl.h"

namespace IG
{
	std::pair<SDL_GLContext, SDL::Renderer*> Initialise(int width, int height, Util::Colour clear)
	{
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

		SDL::Window* window = new SDL::Window("connect", io.DisplaySize.x, io.DisplaySize.y, true);
		SDL_GLContext glcontext = SDL_GL_CreateContext(window->sdlWin);

		// Setup ImGui binding
		ImGui_ImplSdl_Init(window->sdlWin);

		SDL::Renderer* renderer = new SDL::Renderer(window, glcontext, clear);

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






























