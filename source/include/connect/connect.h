// connect.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>

#include "inputmanager.h"
namespace Rx
{
	struct Renderer;
}

namespace Connect
{
	struct GameState
	{
		Input::State inputState;
	};

	void Render(GameState& s, float delta, Rx::Renderer* ren);
	void Update(GameState& s, float delta);
}
