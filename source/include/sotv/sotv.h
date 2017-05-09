// connect.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once


#include <assert.h>
#include <stdint.h>
#include "inputmanager.h"

#include "sotv/station.h"

namespace Rx
{
	struct Renderer;
}

namespace Sotv
{
	// mainly bookkeeping things
	struct Window;

	struct GameState
	{
		Input::State inputState;
		std::vector<Window*> windowList;


		Station* playerStation = 0;
	};

	void Render(GameState& s, float delta, Rx::Renderer* ren);
	void Update(GameState& s, float delta);














	// state of the game itself
}



















