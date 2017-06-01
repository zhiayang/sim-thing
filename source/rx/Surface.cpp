// Surface.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "assetloader.h"
#include "rx/misc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace rx
{
	Surface::Surface(std::string path) : Surface(AssetLoader::Load(path.c_str()))
	{
	}

	Surface::Surface(uint8_t* bytes, size_t w, size_t h, ImageFormat format)
	{
		this->data = bytes;
		this->width = w;
		this->height = h;
		this->format = format;
	}

	Surface::Surface(AssetLoader::Asset* ass)
	{
		using namespace AssetLoader;

		switch(ass->type)
		{
			case AssetType::ImagePNG: {

				int w = 0; int h = 0; int ch = 0;
				this->data = stbi_load_from_memory(ass->raw, ass->length, &w, &h, &ch, 0);

				if(ch == 3)			this->format = ImageFormat::RGB;
				else if(ch == 4)	this->format = ImageFormat::RGBA;
				else				ERROR("Unsupported PNG texture with %d channels per pixel", ch);

				this->width = w;
				this->height = h;

			} break;

			default:
				ERROR("Cannot create surface from unknown type of asset");
		}
	}

	Surface::~Surface()
	{
	}
}





























