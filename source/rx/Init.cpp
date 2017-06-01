// Init.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>

#include "config.h"
#include "rx.h"

#include <SDL2/SDL.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

namespace rx
{
	std::pair<SDL_GLContext, rx::Window*> Initialise(int width, int height)
	{
		// initialise SDL
		{
			auto err = SDL_Init(SDL_INIT_EVERYTHING);
			if(err)	ERROR("SDL failed to initialise, subsystem flags: %d", SDL_INIT_EVERYTHING);
		}


		// Setup window
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_CaptureMouse((SDL_bool) true);
		SDL_GL_SetSwapInterval(1);


		rx::Window* window = new rx::Window("connect", Config::getResX(), Config::getResY(), true);
		SDL_GLContext glcontext = SDL_GL_CreateContext(window->sdlWin);
		if(glcontext == 0)
			ERROR("Failed to create OpenGL context. SDL Error: '%s'", SDL_GetError());

		glbinding::Binding::initialize();

		return { glcontext, window };
	}





	Window::Window(std::string title, int w, int h, bool resizeable) : width(w), height(h)
	{
		this->sdlWin = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | (resizeable ? SDL_WINDOW_RESIZABLE : 0));

		if(!this->sdlWin) ERROR("Failed to initialise SDL Window! (%dx%d)", w, h);

		LOG("Created new SDL Window with dimensions %dx%d", this->width, this->height);
	}

	Window::~Window()
	{
		SDL_DestroyWindow(this->sdlWin);
	}
}






























