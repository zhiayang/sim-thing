// InputManager.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "input.h"
#include "platform.h"

#include "rx/misc.h"

#include <assert.h>

#include <bitset>
#include <algorithm>

namespace input
{
	static void processHandlersForKey(State* state, Key k, bool prev, bool down)
	{
		// ok, fire all the handlers
		// it's already sorted by priority.
		for(auto handler : state->handlers[k])
		{
			bool result = false;

			// do the while things
			if(std::get<1>(handler) == HandlerKind::WhileDown && down)
				result = std::get<3>(handler)(state, k, 0);

			else if(std::get<1>(handler) == HandlerKind::WhileUp && !down)
				result = std::get<3>(handler)(state, k, 0);

			// do the press things
			else if(std::get<1>(handler) == HandlerKind::PressDown && !prev && down)
				result = std::get<3>(handler)(state, k, 0);

			else if(std::get<1>(handler) == HandlerKind::PressUp && prev && !down)
				result = std::get<3>(handler)(state, k, 0);

			if(result) break;
		}
	}

	bool processEvent(State* state, Event event)
	{
		static bool firstMouse = true;
		if(event.ignore) return false;

		using ET = Event::Type;
		switch(event.type)
		{
			case ET::KeyUp:				// fallthrough
			case ET::KeyDown:			// fallthrough
			case ET::MouseButtonDown:	// fallthrough
			case ET::MouseButtonUp: {

				Key k = event.keypress;
				assert(k < Key::NUM_KEYS);

				bool eventdown = (event.type == ET::KeyDown);

				bool prev = state->keys.test((size_t) k);
				if(k != Key::INVALID)
				{
					if(eventdown)	state->keys.set((size_t) k);
					else			state->keys.reset((size_t) k);
				}

				bool down = state->keys.test((size_t) k);
				processHandlersForKey(state, k, prev, down);

			} break;

			case ET::MouseMotion: {
				if(!firstMouse)
				{
					auto old = state->mousePosition;

					state->mousePosition = lx::vec2(event.motionX, event.motionY);
					state->mouseDelta = state->mousePosition - old;
				}
				else
				{
					state->mousePosition = lx::vec2(event.motionX, event.motionY);
					state->mouseDelta = lx::vec2(0);
				}

				firstMouse = false;
			}





			case ET::WindowResize: // do nothing
				break;

			case ET::QuitProgram:
				return true;

			case ET::Invalid:
				ERROR("Invalid event type '%d'", event.type);
		}

		return false;
	}

	lx::vec2 getMousePos(State* state)
	{
		return state->mousePosition;
	}

	lx::vec2 getMouseChange(State* state)
	{
		return state->mouseDelta;
	}

	void Update(State* st, rx::Window* window, double delta)
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

		if(platform::getMousePosition(window->platformData) == st->mousePosition)
			st->mouseDelta = lx::vec2(0);
	}















	static id_t uniqueID = 0;
	using HF_t = std::function<bool(State*, Key, double)>;

	id_t addMouseHandler(State* state, int priority, std::function<bool(State*, double)> handler)
	{
		id_t id = uniqueID++;

		state->mouseHandlers.push_back({ id, priority, handler });

		// sort reverse -- so highest priority is in front
		std::sort(state->mouseHandlers.begin(), state->mouseHandlers.end(), [](const std::tuple<id_t, int,
			std::function<bool(State*, double)>>& a, const std::tuple<id_t, int, std::function<bool(State*, double)>>& b) -> bool
		{
			return std::get<1>(a) > std::get<1>(b);
		});

		return id;
	}



	bool testKey(State* state, Key k)
	{
		return state->keys.test((size_t) k);
	}


	std::vector<id_t> addKeyHandler(State* state, Key key, int priority, std::function<bool(State*, Key, double)> handler,
		HandlerKind kind)
	{
		return addKeyHandler(state, std::vector<Key> { key }, priority, handler, kind);
	}


	std::vector<id_t> addKeyHandler(State* state, std::vector<Key> keys, int priority, HF_t handler, HandlerKind kind)
	{
		std::vector<id_t> ids;
		for(auto key : keys)
		{
			id_t id = uniqueID++;

			state->handlers[key].push_back({ id, kind, priority, handler });

			// sort reverse -- so highest priority is in front
			std::sort(state->handlers[key].begin(), state->handlers[key].end(), [](const std::tuple<id_t, HandlerKind, int, HF_t>& a,
				const std::tuple<id_t, HandlerKind, int, HF_t>& b) -> bool
			{
				return std::get<2>(a) > std::get<2>(b);
			});

			ids.push_back(id);
		}

		return ids;
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




















