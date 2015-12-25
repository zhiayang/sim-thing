// AssetLoader.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <string>
#include <stdint.h>
#include "SDL2/SDL.h"

namespace AssetLoader
{
	enum class AssetType
	{
		Unknown,
		ImagePNG,
	};

	struct Asset
	{
		uint8_t* raw;
		AssetType type;
		std::string path;
		SDL_RWops* sdlrw;
	};

	std::string getResourcePath();
	Asset* Load(const char* path);
	void Unload(Asset* asset);
}
