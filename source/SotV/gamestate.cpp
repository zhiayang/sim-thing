// gamestate.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "imgui.h"
#include "rx.h"

#include "sotv/sotv.h"
#include "sotv/gui.h"

namespace Sotv
{
	void Render(GameState& gs, double delta, rx::Renderer* ren)
	{
		// render background stuff first
		gs.playerStation->Render(gs, delta, ren);

		// then windows
		for(auto w : gs.windowList)
			w->Render(gs, delta, ren);
	}

	void Update(GameState& gs, double delta)
	{
		gs.playerStation->Update(gs, delta);

		for(auto w : gs.windowList)
			w->Update(gs, delta);
	}
}
