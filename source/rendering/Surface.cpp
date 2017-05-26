// Surface.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "renderer/rx.h"

#include <SDL2/SDL_image.h>

namespace Rx
{
	Surface::Surface(std::string path) : Surface(AssetLoader::Load(path.c_str()))
	{
	}

	Surface::Surface(AssetLoader::Asset* ass)
	{
		using namespace AssetLoader;

		assert(ass);
		this->asset = ass;

		switch(ass->type)
		{
			case AssetType::ImagePNG:
				this->sdlSurf = IMG_LoadPNG_RW(this->asset->sdlrw);
				if(!this->sdlSurf)
					ERROR("Failed to load PNG image from asset '%s'", this->asset->path.c_str());

				break;

			default:
				ERROR("Cannot create surface from unknown type of asset");
		}
	}

	Surface::~Surface()
	{
		SDL_FreeSurface(this->sdlSurf);

		if(this->asset)
			AssetLoader::Unload(this->asset);
	}


	Surface::Surface(SDL_Surface* sdls)
	{
		this->asset = 0;
		this->sdlSurf = sdls;
	}
}





























