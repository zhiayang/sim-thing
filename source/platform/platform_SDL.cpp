// platform_SDL.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <string>
#include <assert.h>

#include "platform.h"
#include "platform/sdl.h"

namespace platform
{
	static input::Key translateSDLKeyToKey(uint32_t sdlk);
	static input::Key translateSDLMouseButtonToKey(uint32_t sdlm);

	void* initialiseUserData()
	{
		SDLUserData* userdata = new SDLUserData();

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

		// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		SDL_CaptureMouse((SDL_bool) true);

		SDL_ShowCursor(false);

		SDL_GL_SetSwapInterval(1);

		return userdata;
	}

	void uninitialiseUserData(void* userdata)
	{
		delete (SDLUserData*) userdata;
	}

	void endProgram()
	{
		SDL_Quit();
	}


	void* createWindow(void* userdata, std::string name, int width, int height, bool resize)
	{
		SDL_Window* sdlWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | (resize ? SDL_WINDOW_RESIZABLE : 0));

		if(!sdlWindow)
			ERROR("Failed to initialise SDL Window! (%dx%d)", width, height);

		LOG("Created new SDL Window with dimensions %dx%d", width, height);

		SDLUserData* sud = (SDLUserData*) userdata;

		sud->glContext = SDL_GL_CreateContext(sdlWindow);
		if(sud->glContext == 0)
			ERROR("Failed to create OpenGL context. SDL Error: '%s'", SDL_GetError());

		SDL_SetWindowGrab(sdlWindow, SDL_TRUE);

		int w = 0;
		int h = 0;

		SDL_GetWindowSize(sdlWindow, &w, &h);
		SDL_WarpMouseInWindow(sdlWindow, w / 2, h / 2);


		return sdlWindow;
	}

	int getWindowScale(void* userdata, void* userwindow)
	{
		int rx = 0; int ry = 0;
		SDL_GetWindowSize((SDL_Window*) userwindow, &rx, &ry);
		LOG("window is %d x %d", rx, ry);

		int dx = 0; int dy = 0;
		SDL_GL_GetDrawableSize((SDL_Window*) userwindow, &dx, &dy);

		int sx = dx / rx;
		int sy = dy / ry;

		assert(sx == sy && "vertical and horizontal display scaling values do not match");

		return sx;
	}

	void destroyWindow(void* userdata, void* userwindow)
	{
		SDL_GL_DeleteContext(((SDLUserData*) userdata)->glContext);
		SDL_DestroyWindow((SDL_Window*) userwindow);
	}





	void preFrame(void* userdata, void* userwindow)
	{
	}

	void beginFrame(void* userdata, void* userwindow)
	{
	}

	void endFrame(void* userdata, void* userwindow)
	{
		SDL_GL_SwapWindow((SDL_Window*) userwindow);
		// int w = 0;
		// int h = 0;

		// SDL_GetWindowSize((SDL_Window*) userwindow, &w, &h);
		// SDL_WarpMouseInWindow((SDL_Window*) userwindow, w / 2, h / 2);
	}






	// std::pair<SDL_Event, bool> ProcessEvents()
	// {
	// 	bool done = false;

	// 	SDL_Event event;
	// 	while(SDL_PollEvent(&event))
	// 	{
	// 		if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
	// 			Input::handleKeyInput(&gameState->inputState, &event);

	// 		else if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
	// 			Input::handleMouseInput(&gameState->inputState, &event);

	// 		if(event.type == SDL_QUIT)
	// 		{
	// 			done = true;
	// 		}
	// 		else if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	// 		{
	// 			assert(theRenderer);
	// 			theRenderer->updateWindowSize(event.window.data1, event.window.data2);
	// 		}
	// 	}

	// 	return { event, done };
	// }



	// input stuff
	bool hasEvent(void* userdata)
	{
		return SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	}

	std::vector<input::Event> getEvents(void* userdata)
	{
		using ET = input::Event::Type;
		std::vector<input::Event> eventlist;

		SDL_Event sdle;
		while(SDL_PollEvent(&sdle))
		{
			input::Event event;
			switch(sdle.type)
			{
				case SDL_KEYUP: {
					event.type = ET::KeyUp;
					event.keypress = translateSDLKeyToKey(sdle.key.keysym.sym);

					if(sdle.key.repeat)
						event.ignore = true;

				} break;

				case SDL_KEYDOWN: {
					event.type = ET::KeyDown;
					event.keypress = translateSDLKeyToKey(sdle.key.keysym.sym);

					if(sdle.key.repeat)
						event.ignore = true;

				} break;

				case SDL_MOUSEBUTTONUP: {
					event.type = ET::MouseButtonUp;
					event.keypress = translateSDLMouseButtonToKey(sdle.button.button);

				} break;

				case SDL_MOUSEBUTTONDOWN: {
					event.type = ET::MouseButtonDown;
					event.keypress = translateSDLMouseButtonToKey(sdle.button.button);

				} break;

				case SDL_MOUSEMOTION: {
					event.type = ET::MouseMotion;
					event.motionX = sdle.motion.x;
					event.motionY = sdle.motion.y;

					auto win = SDL_GetWindowFromID(sdle.motion.windowID);
					int w = 0;
					int h = 0;

					SDL_GetWindowSize(win, &w, &h);
					SDL_WarpMouseInWindow(win, w / 2, h / 2);
					// SDL_Mouse

				} break;

				case SDL_WINDOWEVENT_SIZE_CHANGED: {
					event.type = ET::WindowResize;
					event.windowWidth = sdle.window.data1;
					event.windowHeight = sdle.window.data2;

				} break;

				case SDL_QUIT: {
					event.type = ET::QuitProgram;
				} break;

				default:
					event.type = ET::Invalid;
					event.ignore = true;
					break;
			}

			eventlist.push_back(event);
		}

		return eventlist;
	}


	lx::vec2 getMousePosition(void* userdata)
	{
		int mx = 0;
		int my = 0;

		SDL_GetMouseState(&mx, &my);

		return lx::vec2(mx, my);
	}

	void updateInput(input::State* inputState, double delta, void* userdata)
	{
		// do nothing
	}


















	static input::Key translateSDLMouseButtonToKey(uint32_t sdlm)
	{
		using namespace input;
		switch(sdlm)
		{
			case SDL_BUTTON_LEFT:	return Key::MouseL;
			case SDL_BUTTON_RIGHT:	return Key::MouseR;
			case SDL_BUTTON_MIDDLE:	return Key::MouseMiddle;

			default:
				WARN("invalid mouse button pressed");
				return Key::INVALID;
		}
	}

	static input::Key translateSDLKeyToKey(uint32_t sdlKeycode)
	{
		using namespace input;
		switch(sdlKeycode)
		{
			case SDLK_a:            return Key::A;
			case SDLK_b:            return Key::B;
			case SDLK_c:            return Key::C;
			case SDLK_d:            return Key::D;
			case SDLK_e:            return Key::E;
			case SDLK_f:            return Key::F;
			case SDLK_g:            return Key::G;
			case SDLK_h:            return Key::H;
			case SDLK_i:            return Key::I;
			case SDLK_j:            return Key::J;
			case SDLK_k:            return Key::K;
			case SDLK_l:            return Key::L;
			case SDLK_m:            return Key::M;
			case SDLK_n:            return Key::N;
			case SDLK_o:            return Key::O;
			case SDLK_p:            return Key::P;
			case SDLK_q:            return Key::Q;
			case SDLK_r:            return Key::R;
			case SDLK_s:            return Key::S;
			case SDLK_t:            return Key::T;
			case SDLK_u:            return Key::U;
			case SDLK_v:            return Key::V;
			case SDLK_w:            return Key::W;
			case SDLK_x:            return Key::X;
			case SDLK_y:            return Key::Y;
			case SDLK_z:            return Key::Z;

			case SDLK_0:            return Key::Zero;
			case SDLK_1:            return Key::One;
			case SDLK_2:            return Key::Two;
			case SDLK_3:            return Key::Three;
			case SDLK_4:            return Key::Four;
			case SDLK_5:            return Key::Five;
			case SDLK_6:            return Key::Six;
			case SDLK_7:            return Key::Seven;
			case SDLK_8:            return Key::Eight;
			case SDLK_9:            return Key::Nine;

			case SDLK_COMMA:        return Key::Comma;
			case SDLK_PERIOD:       return Key::Period;
			case SDLK_LEFTBRACKET:  return Key::LSquare;
			case SDLK_RIGHTBRACKET: return Key::RSquare;

			case SDLK_ESCAPE:       return Key::Escape;
			case SDLK_LGUI:         return Key::SuperL;
			case SDLK_RGUI:         return Key::SuperR;
			case SDLK_LCTRL:        return Key::ControlL;
			case SDLK_RCTRL:        return Key::ControlR;
			case SDLK_LALT:         return Key::AltL;
			case SDLK_RALT:         return Key::AltR;
			case SDLK_LSHIFT:       return Key::ShiftL;
			case SDLK_RSHIFT:       return Key::ShiftR;
			case SDLK_SPACE:        return Key::Space;

			default:
				LOG("Fed invalid SDL keycode into %s", __PRETTY_FUNCTION__);
				return Key::INVALID;
		}
	}
}
















