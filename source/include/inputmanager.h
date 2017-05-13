// InputManager.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include "utilities.h"

#include <functional>
#include <unordered_map>

#include <SDL2/SDL.h>

namespace Input
{
	enum class Keys
	{
		INVALID,

		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		Zero,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,

		Escape,
		SuperL,
		SuperR,

		ControlL,
		ControlR,

		AltL,
		AltR,

		ShiftL,
		ShiftR,

		Space,

		NUM_KEYS,
	};

	Keys FromSDL(uint32_t sdlKeycode);
	uint32_t ToSDL(Keys key);

	constexpr size_t NUM_KEYS = (size_t) Keys::NUM_KEYS;

	enum class HandlerKind
	{
		INVALID,
		WhileUp,		// fires while the key is up
		WhileDown,		// fires while the key is down

		PressUp,		// fires once, when the key goes from down to up.
		PressDown,		// fires once, when the key goes from up to down.

		// note that when a key is pressed, both 'WhileDown' and 'PressDown' will fire, and similar for when a key is released.

		INVALID_2
	};

	struct State
	{
		std::bitset<NUM_KEYS> keys;

		std::unordered_map<Keys, std::vector<std::tuple<id_t, HandlerKind, int, std::function<bool(State*, Keys)>>>> handlers;
	};

	void HandleInput(State* inputState, SDL_Event* e);

	bool testKey(State* state, Keys k);

	// returns a unique ID that can be used to remove the handler later.
	// handlers with higher priorities are fired first.

	// handler spec: returns a bool that determines if the key event was handled
	// if true, then the key is *not* passed on; if false, then subsequent handlers are fired.

	id_t addHandler(State* state, Keys k, int priority, std::function<bool(State*, Keys)> handler, HandlerKind kind);
	void removeHandler(State* state, Keys k, id_t handler);
}













