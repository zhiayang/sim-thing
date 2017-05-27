// InputManager.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include "utilities.h"

#include <functional>
#include <unordered_map>

#include <SDL2/SDL.h>

#include <glm/vec2.hpp>

namespace Input
{
	enum class Key
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



		MouseL,
		MouseR,
		MouseMiddle,

		NUM_KEYS,
	};

	Key FromSDL(uint32_t sdlKeycode);
	uint32_t ToSDL(Key key);

	constexpr size_t NUM_KEYS = (size_t) Key::NUM_KEYS;

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
		glm::vec2 mouseDelta;
		glm::vec2 mousePosition;

		std::bitset<NUM_KEYS> keys;

		std::unordered_map<Key, std::vector<std::tuple<id_t, HandlerKind, int, std::function<bool(State*, Key, double)>>>> handlers;
	};

	void handleKeyInput(State* inputState, SDL_Event* e);
	void handleMouseInput(State* inputState, SDL_Event* e);

	bool testKey(State* state, Key k);

	void Update(State* inputState, double delta);


	// returns a unique ID that can be used to remove the handler later.
	// handlers with higher priorities are fired first.

	// handler spec: returns a bool that determines if the key event was handled
	// if true, then the key is *not* passed on; if false, then subsequent handlers are fired.

	id_t addKeyHandler(State* state, Key k, int priority, std::function<bool(State*, Key, double)> handler, HandlerKind kind);
	void removeKeyHandler(State* state, Key k, id_t handler);

	glm::vec2 getMousePos(State* state);
	glm::vec2 getMouseChange(State* state);
}













