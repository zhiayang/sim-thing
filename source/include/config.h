// Config.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>

namespace Config
{
	uint64_t getResX();
	void setResX(uint64_t x);

	uint64_t getResY();
	void setResY(uint64_t y);

	bool getVSyncEnabled();
	void setVSyncEnabled(bool enabled);

	bool getShowFps();
	void setShowFps(bool show);
}
