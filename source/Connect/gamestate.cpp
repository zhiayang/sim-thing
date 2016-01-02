// gamestate.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "imgui.h"
#include "graphicswrapper.h"

#include "connect/connect.h"
#include "connect/gui.h"

namespace Connect
{
	void Render(GameState& gs, float delta, Rx::Renderer* ren)
	{
		// render background stuff first
		// TODO

		// then windows
		for(auto w : gs.windowList)
			w->Render(gs, delta, ren);
	}

	void Update(GameState& gs, float delta)
	{
	}
}
