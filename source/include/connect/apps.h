// apps.h
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
	struct TerminalState
	{
		struct TabState
		{
			std::string title;
			std::string curCmd;
			std::vector<std::string> scrollback;
		};


		TerminalState();





		std::vector<TabState> tabs;



		// terminals can be mirrored to each other
		// maintain a refcount, so we don't need to make multiple instances
		// of TerminalState, but won't leak this object

		void ref();
		void deref();

		size_t _refcount;
	};
}











































