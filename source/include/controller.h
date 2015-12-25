// Controller.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <thread>

#include "utilities.h"
#include "inputmanager.h"

#include "gravity/gravity.h"

namespace SDL { struct Renderer; }


struct TimeInfo
{
	double sec()
	{
		return this->ms() / 1000.0;
	}

	double ms()
	{
		return this->µs() / 1000.0;
	}

	double µs()
	{
		return this->ns() / 1000.0;
	}

	uint64_t ns()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
};


struct Controller
{
	public:
		Controller();

		void cleanup();
		void start();

		void UpdateLoop();
		void RenderLoop();
		bool checkSDLEventQueue();

		bool isRunning() { return this->run; }
		Controller* instance() { return this; }

	private:
		Gravity::SceneController*	sceneController;
		TimeInfo		timeInfo;
		Input::State	inputState;
		SDL::Window*	window;
		SDL::Renderer*	renderer;
		bool			run;
		std::thread		updateLoop;
		SDL::Font*		debugFont;
		SDL_GLContext	oglContext;
};





