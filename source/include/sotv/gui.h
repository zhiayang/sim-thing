// gui.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

#include "sotv/widgets.h"

namespace Rx
{
	struct Renderer;
};

namespace Sotv
{
	struct GameState;

	struct Window
	{
		Window();
		virtual ~Window();

		virtual void Close(GameState& gs) final;
		virtual bool isOpen() final;

		virtual void Render(GameState& gs, double delta, Rx::Renderer* r);
		virtual void Update(GameState& gs, double delta);

		bool isWindowOpen = 0;
		std::string name;
		size_t id = 0;
	};
}











































