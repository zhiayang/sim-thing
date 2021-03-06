// AssetLoader.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <string>
#include <stdint.h>
#include "SDL2/SDL.h"

#include <glbinding/gl/types.h>

namespace AssetLoader
{
	enum class AssetType
	{
		Unknown,
		ImagePNG,
		ImageDDS,

		ShaderFrag,
		ShaderVert,

		ModelOBJ,
	};

	struct Asset
	{
		uint8_t* raw;
		size_t length;

		AssetType type;
		std::string path;
		SDL_RWops* sdlrw;
	};

	std::string getResourcePath();
	Asset* Load(std::string path);
	void Unload(Asset* asset);
}
