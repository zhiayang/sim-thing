// connect.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once


#include <assert.h>
#include <stdint.h>
#include "input.h"

#include "sotv/station.h"

namespace rx
{
	struct Renderer;
}

namespace Sotv
{
	// mainly bookkeeping things
	struct Window;

	struct GameState
	{
		input::State inputState;
		std::vector<Window*> windowList;


		Station* playerStation = 0;
	};

	void Render(GameState& s, double delta, rx::Renderer* ren);
	void Update(GameState& s, double delta);














	// state of the game itself
}



















