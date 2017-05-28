// Surface.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "assetloader.h"
#include "renderer/misc.h"

namespace Rx
{
	Surface::Surface(std::string path) : Surface(AssetLoader::Load(path.c_str()))
	{
	}

	Surface::Surface(AssetLoader::Asset* ass)
	{
		using namespace AssetLoader;

		switch(ass->type)
		{
			case AssetType::ImagePNG:

				break;

			default:
				ERROR("Cannot create surface from unknown type of asset");
		}
	}

	Surface::~Surface()
	{
	}
}





























