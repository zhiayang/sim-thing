// gui.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

namespace Rx
{
	struct Renderer;
};

namespace Connect
{
	struct GameState;
	struct TerminalState;

	struct Window
	{
		Window();
		virtual ~Window();

		virtual void Close(GameState& gs) final;
		virtual bool isOpen() final;

		virtual void Render(GameState& gs, float delta, Rx::Renderer* r);
		virtual void Update(GameState& gs, float delta);

		bool isWindowOpen = 0;
		std::string name;
		size_t id = 0;
	};

	struct TerminalWindow : Window
	{
		TerminalWindow();
		TerminalWindow(TerminalState* ts);

		virtual ~TerminalWindow();
		virtual void Render(GameState& gs, float delta, Rx::Renderer* r) override;
		virtual void Update(GameState& gs, float delta) override;

		TerminalState* tstate = 0;
	};
}











































