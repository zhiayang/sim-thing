// InputManager.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "inputmanager.h"
#include <SDL2/SDL.h>

#include <assert.h>

#include <bitset>
#include <algorithm>

namespace Input
{
	void handleKeyInput(State* inputState, SDL_Event* e)
	{
		assert(e->type == SDL_KEYDOWN || e->type == SDL_KEYUP);
		if(e->key.repeat)
			return;

		{
			Key k = FromSDL(e->key.keysym.sym);

			assert(k < Key::NUM_KEYS);

			bool prev = inputState->keys.test((size_t) k);
			if(k != Key::INVALID)
			{
				if(e->type == SDL_KEYDOWN)
					inputState->keys.set((size_t) k);

				else if(e->type == SDL_KEYUP)
					inputState->keys.reset((size_t) k);
			}

			bool down = inputState->keys.test((size_t) k);

			// ok, fire all the handlers
			// it's already sorted by priority.
			for(auto handler : inputState->handlers[k])
			{
				bool result = false;

				// do the while things
				if(std::get<1>(handler) == HandlerKind::WhileDown && down)
					result = std::get<3>(handler)(inputState, k, 0);

				else if(std::get<1>(handler) == HandlerKind::WhileUp && !down)
					result = std::get<3>(handler)(inputState, k, 0);

				// do the press things
				else if(std::get<1>(handler) == HandlerKind::PressDown && !prev && down)
					result = std::get<3>(handler)(inputState, k, 0);

				else if(std::get<1>(handler) == HandlerKind::PressUp && prev && !down)
					result = std::get<3>(handler)(inputState, k, 0);

				if(result) break;
			}
		}
	}

	void handleMouseInput(State* inputState, SDL_Event* e)
	{
		if(e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEBUTTONDOWN)
		{
			// mouse buttons
			Key k = Key::INVALID;
			switch(e->button.button)
			{
				case SDL_BUTTON_LEFT:	k = Key::MouseL; break;
				case SDL_BUTTON_RIGHT:	k = Key::MouseR; break;
				case SDL_BUTTON_MIDDLE:	k = Key::MouseMiddle; break;
				default:				WARN("invalid mouse button pressed");
			}

			bool prev = inputState->keys.test((size_t) k);
			if(k != Key::INVALID)
			{
				if(e->type == SDL_MOUSEBUTTONDOWN)
					inputState->keys.set((size_t) k);

				else if(e->type == SDL_MOUSEBUTTONUP)
					inputState->keys.reset((size_t) k);
			}

			bool down = inputState->keys.test((size_t) k);

			// fire all the handlers
			for(auto handler : inputState->handlers[k])
			{
				bool result = false;

				// do the while things
				if(std::get<1>(handler) == HandlerKind::WhileDown && down)
					result = std::get<3>(handler)(inputState, k, 0);

				else if(std::get<1>(handler) == HandlerKind::WhileUp && !down)
					result = std::get<3>(handler)(inputState, k, 0);

				// do the press things
				else if(std::get<1>(handler) == HandlerKind::PressDown && !prev && down)
					result = std::get<3>(handler)(inputState, k, 0);

				else if(std::get<1>(handler) == HandlerKind::PressUp && prev && !down)
					result = std::get<3>(handler)(inputState, k, 0);

				if(result) break;
			}
		}
		else
		{
			assert(e->type == SDL_MOUSEMOTION);

			auto old = inputState->mousePosition;

			inputState->mousePosition = glm::vec2(e->motion.x, e->motion.y);
			inputState->mouseDelta = inputState->mousePosition - old;
		}
	}

	glm::vec2 getMousePos(State* state)
	{
		return state->mousePosition;
	}

	glm::vec2 getMouseChange(State* state)
	{
		return state->mouseDelta;
	}

	void Update(State* st, double delta)
	{
		// no choice but to loop through all the handlers
		for(auto key : st->handlers)
		{
			for(auto handler : key.second)
			{
				bool down = testKey(st, key.first);
				bool result = false;

				if(std::get<1>(handler) == HandlerKind::WhileDown && down)
					result = std::get<3>(handler)(st, key.first, delta);

				else if(std::get<1>(handler) == HandlerKind::WhileUp && !down)
					result = std::get<3>(handler)(st, key.first, delta);
			}
		}

		int mx = 0;
		int my = 0;

		SDL_GetMouseState(&mx, &my);
		if(glm::vec2(mx, my) == st->mousePosition)
			st->mouseDelta = glm::vec2(0);
	}





















	Key FromSDL(uint32_t sdlKeycode)
	{
		switch(sdlKeycode)
		{
			case  SDLK_a:		return Key::A;
			case  SDLK_b:		return Key::B;
			case  SDLK_c:		return Key::C;
			case  SDLK_d:		return Key::D;
			case  SDLK_e:		return Key::E;
			case  SDLK_f:		return Key::F;
			case  SDLK_g:		return Key::G;
			case  SDLK_h:		return Key::H;
			case  SDLK_j:		return Key::J;
			case  SDLK_k:		return Key::K;
			case  SDLK_l:		return Key::L;
			case  SDLK_m:		return Key::M;
			case  SDLK_n:		return Key::N;
			case  SDLK_o:		return Key::O;
			case  SDLK_p:		return Key::P;
			case  SDLK_q:		return Key::Q;
			case  SDLK_r:		return Key::R;
			case  SDLK_s:		return Key::S;
			case  SDLK_t:		return Key::T;
			case  SDLK_u:		return Key::U;
			case  SDLK_v:		return Key::V;
			case  SDLK_w:		return Key::W;
			case  SDLK_x:		return Key::X;
			case  SDLK_y:		return Key::Y;
			case  SDLK_z:		return Key::Z;

			case  SDLK_0:		return Key::Zero;
			case  SDLK_1:		return Key::One;
			case  SDLK_2:		return Key::Two;
			case  SDLK_3:		return Key::Three;
			case  SDLK_4:		return Key::Four;
			case  SDLK_5:		return Key::Five;
			case  SDLK_6:		return Key::Six;
			case  SDLK_7:		return Key::Seven;
			case  SDLK_8:		return Key::Eight;
			case  SDLK_9:		return Key::Nine;

			case  SDLK_ESCAPE:	return Key::Escape;
			case  SDLK_LGUI:	return Key::SuperL;
			case  SDLK_RGUI:	return Key::SuperR;
			case  SDLK_LCTRL:	return Key::ControlL;
			case  SDLK_RCTRL:	return Key::ControlR;
			case  SDLK_LALT:	return Key::AltL;
			case  SDLK_RALT:	return Key::AltR;
			case  SDLK_LSHIFT:	return Key::ShiftL;
			case  SDLK_RSHIFT:	return Key::ShiftR;
			case  SDLK_SPACE:	return Key::Space;

			default:
				LOG("Fed invalid SDL keycode into %s", __PRETTY_FUNCTION__);
				return Key::INVALID;
		}
	}

	uint32_t ToSDL(Key key)
	{
		return (uint32_t) key;
	}




	bool testKey(State* state, Key k)
	{
		return state->keys.test((size_t) k);
	}


	using HF_t = std::function<bool(State*, Key, double)>;
	id_t addKeyHandler(State* state, Key key, int priority, HF_t handler, HandlerKind kind)
	{
		static id_t uniqueID = 0;
		id_t id = uniqueID++;

		state->handlers[key].push_back({ id, kind, priority, handler });

		// sort reverse -- so highest priority is in front
		std::sort(state->handlers[key].begin(), state->handlers[key].end(), [](const std::tuple<id_t, HandlerKind, int, HF_t>& a,
			const std::tuple<id_t, HandlerKind, int, HF_t>& b) -> bool
		{
			return std::get<2>(a) > std::get<2>(b);
		});

		return id;
	}

	void removeKeyHandler(State* state, Key k, id_t id)
	{
		if(state->handlers.find(k) != state->handlers.end())
		{
			for(auto it = state->handlers[k].begin(); it != state->handlers[k].end(); it++)
			{
				if(std::get<0>(*it) == id)
				{
					state->handlers[k].erase(it);
					return;
				}
			}
		}

		ERROR("Handler with id '%zu' was not found", id);
	}
}





















