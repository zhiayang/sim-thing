// InputManager.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>
#include "inputmanager.h"

#include <bitset>
#include <algorithm>

namespace Input
{
	void HandleInput(State* inputState, SDL_Event* e)
	{
		assert(e->type == SDL_KEYDOWN || e->type == SDL_KEYUP);
		Keys k = FromSDL(e->key.keysym.sym);

		assert(k < Keys::NUM_KEYS);

		bool prev = inputState->keys.test((size_t) k);
		if(k != Keys::INVALID)
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
				result = std::get<3>(handler)(inputState, k);

			else if(std::get<1>(handler) == HandlerKind::WhileUp && !down)
				result = std::get<3>(handler)(inputState, k);

			// do the press things
			else if(std::get<1>(handler) == HandlerKind::PressDown && !prev && down)
				result = std::get<3>(handler)(inputState, k);

			else if(std::get<1>(handler) == HandlerKind::PressUp && prev && !down)
				result = std::get<3>(handler)(inputState, k);

			if(result) break;
		}
	}

	Keys FromSDL(uint32_t sdlKeycode)
	{
		switch(sdlKeycode)
		{
			case  SDLK_a:		return Keys::A;
			case  SDLK_b:		return Keys::B;
			case  SDLK_c:		return Keys::C;
			case  SDLK_d:		return Keys::D;
			case  SDLK_e:		return Keys::E;
			case  SDLK_f:		return Keys::F;
			case  SDLK_g:		return Keys::G;
			case  SDLK_h:		return Keys::H;
			case  SDLK_j:		return Keys::J;
			case  SDLK_k:		return Keys::K;
			case  SDLK_l:		return Keys::L;
			case  SDLK_m:		return Keys::M;
			case  SDLK_n:		return Keys::N;
			case  SDLK_o:		return Keys::O;
			case  SDLK_p:		return Keys::P;
			case  SDLK_q:		return Keys::Q;
			case  SDLK_r:		return Keys::R;
			case  SDLK_s:		return Keys::S;
			case  SDLK_t:		return Keys::T;
			case  SDLK_u:		return Keys::U;
			case  SDLK_v:		return Keys::V;
			case  SDLK_w:		return Keys::W;
			case  SDLK_x:		return Keys::X;
			case  SDLK_y:		return Keys::Y;
			case  SDLK_z:		return Keys::Z;

			case  SDLK_0:		return Keys::Zero;
			case  SDLK_1:		return Keys::One;
			case  SDLK_2:		return Keys::Two;
			case  SDLK_3:		return Keys::Three;
			case  SDLK_4:		return Keys::Four;
			case  SDLK_5:		return Keys::Five;
			case  SDLK_6:		return Keys::Six;
			case  SDLK_7:		return Keys::Seven;
			case  SDLK_8:		return Keys::Eight;
			case  SDLK_9:		return Keys::Nine;

			case  SDLK_ESCAPE:	return Keys::Escape;
			case  SDLK_LGUI:	return Keys::SuperL;
			case  SDLK_RGUI:	return Keys::SuperR;
			case  SDLK_LCTRL:	return Keys::ControlL;
			case  SDLK_RCTRL:	return Keys::ControlR;
			case  SDLK_LALT:	return Keys::AltL;
			case  SDLK_RALT:	return Keys::AltR;
			case  SDLK_LSHIFT:	return Keys::ShiftL;
			case  SDLK_RSHIFT:	return Keys::ShiftR;
			case  SDLK_SPACE:	return Keys::Space;

			default:
				LOG("Fed invalid SDL keycode into %s", __PRETTY_FUNCTION__);
				return Keys::INVALID;
		}
	}

	uint32_t ToSDL(Keys key)
	{
		return (uint32_t) key;
	}




	bool testKey(State* state, Keys k)
	{
		return state->keys.test((size_t) k);
	}


	using HF_t = std::function<bool(State*, Keys)>;
	id_t addHandler(State* state, Keys key, int priority, HF_t handler, HandlerKind kind)
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

	void removeHandler(State* state, Keys k, id_t id)
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





















