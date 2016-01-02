// InputManager.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include "utilities.h"
#include <unordered_map>

#include <SDL2/SDL.h>

namespace Input
{
	enum class Keys
	{
		A			= SDLK_a,
		B			= SDLK_b,
		C			= SDLK_c,
		D			= SDLK_d,
		E			= SDLK_e,
		F			= SDLK_f,
		G			= SDLK_g,
		H			= SDLK_h,
		J			= SDLK_j,
		K			= SDLK_k,
		L			= SDLK_l,
		M			= SDLK_m,
		N			= SDLK_n,
		O			= SDLK_o,
		P			= SDLK_p,
		Q			= SDLK_q,
		R			= SDLK_r,
		S			= SDLK_s,
		T			= SDLK_t,
		U			= SDLK_u,
		V			= SDLK_v,
		W			= SDLK_w,
		X			= SDLK_x,
		Y			= SDLK_y,
		Z			= SDLK_z,

		Zero		= SDLK_0,
		One			= SDLK_1,
		Two			= SDLK_2,
		Three		= SDLK_3,
		Four		= SDLK_4,
		Five		= SDLK_5,
		Six			= SDLK_6,
		Seven		= SDLK_7,
		Eight		= SDLK_8,
		Nine		= SDLK_9,

		Escape		= SDLK_ESCAPE,
		SuperL		= SDLK_LGUI,
		SuperR		= SDLK_RGUI,

		ControlL	= SDLK_LCTRL,
		ControlR	= SDLK_RCTRL,

		AltL		= SDLK_LALT,
		AltR		= SDLK_RALT,

		ShiftL		= SDLK_LSHIFT,
		ShiftR		= SDLK_RSHIFT,

		Space		= SDLK_SPACE,

		Unknown		= 0,
	};

	Keys FromSDL(uint32_t sdlKeycode);
	uint32_t ToSDL(Keys key);

	struct State
	{
		std::unordered_map<Keys, bool> keys;
		bool isKeyDown(Keys key) { return this->keys[key]; }
	};

	void HandleInput(State* inputState, SDL_Event* e);
}













