// Config.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "config.h"
namespace Config
{
	// lol such place many holder
	static uint64_t rx = 640;
	static uint64_t ry = 480;
	static bool vsyncEnabled = true;
	static bool showFps = true;

	uint64_t getResX()
	{
		return rx;
	}

	uint64_t getResY()
	{
		return ry;
	}

	void setResX(uint64_t x)
	{
		rx = x;
	}

	void setResY(uint64_t y)
	{
		ry = y;
	}

	bool getVSyncEnabled()
	{
		return vsyncEnabled;
	}

	void setVSyncEnabled(bool enabled)
	{
		vsyncEnabled = enabled;
	}

	bool getShowFps()
	{
		return showFps;
	}

	void setShowFps(bool show)
	{
		showFps = show;
	}
}






