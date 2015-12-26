// Surface.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sdlwrapper.h"
namespace SDL
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

	Surface* Surface::fromText(Util::Font font, Util::Colour colour, std::string txt)
	{
		// create a surface

		SDL_Color sdlc;
		sdlc.r = colour.r;
		sdlc.g = colour.g;
		sdlc.b = colour.b;
		sdlc.a = colour.a;

		SDL_Surface* sdls = TTF_RenderUTF8_Blended(font.sdl, txt.c_str(), sdlc);
		if(!sdls) ERROR("Failed to create surface");

		return new Surface(sdls);
	}
}





























