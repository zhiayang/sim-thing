// input.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include "utilities.h"

#include <functional>
#include <unordered_map>

namespace rx
{
	struct Window;
}

namespace input
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
		I,
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
		lx::vec2 mouseDelta;
		lx::vec2 mousePosition;

		std::bitset<NUM_KEYS> keys;

		std::unordered_map<Key, std::vector<std::tuple<id_t, HandlerKind, int, std::function<bool(State*, Key, double)>>>> handlers;
		std::vector<std::tuple<id_t, int, std::function<bool(State*, double)>>> mouseHandlers;
	};

	struct Event
	{
		enum class Type
		{
			Invalid,

			QuitProgram,

			KeyUp,
			KeyDown,

			MouseButtonUp,
			MouseButtonDown,

			MouseMotion,

			WindowResize,
		};

		Type type = Type::Invalid;
		bool ignore = false;

		// only valid if Event.type is KeyUp, KeyDown, MouseButtonUp, or MouseButtonDown.
		Key keypress = Key::INVALID;

		// only valid if Event.type is MouseMotion
		int motionX = 0;
		int motionY = 0;

		// only valid if Event.type is WindowResize
		int windowWidth = 0;
		int windowHeight = 0;
	};


	bool testKey(State* state, Key k);

	void Update(State* inputState, rx::Window* window, double delta);

	// returns true if we need to quit
	bool processEvent(State* state, Event event);

	// returns a unique ID that can be used to remove the handler later.
	// handlers with higher priorities are fired first.

	// handler spec: returns a bool that determines if the key event was handled
	// if true, then the key is *not* passed on; if false, then subsequent handlers are fired.

	std::vector<id_t> addKeyHandler(State* state, std::vector<Key> keys, int priority, std::function<bool(State*, Key, double)> handler,
		HandlerKind kind);

	std::vector<id_t> addKeyHandler(State* state, Key key, int priority, std::function<bool(State*, Key, double)> handler,
		HandlerKind kind);

	id_t addMouseHandler(State* state, int priority, std::function<bool(State*, double)> handler);

	void removeKeyHandler(State* state, Key key, id_t handler);

	lx::vec2 getMousePos(State* state);
	lx::vec2 getMouseChange(State* state);
}













