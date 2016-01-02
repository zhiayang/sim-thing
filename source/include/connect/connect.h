// connect.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include "inputmanager.h"
namespace Rx
{
	struct Renderer;
}

namespace Connect
{
	struct Window;

	struct GameState
	{
		Input::State inputState;
		std::vector<Window*> windowList;
	};

	void Render(GameState& s, float delta, Rx::Renderer* ren);
	void Update(GameState& s, float delta);
}
