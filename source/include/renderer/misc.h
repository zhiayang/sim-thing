// misc.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>

#include <SDL2/SDL.h>

#include "utilities.h"


namespace AssetLoader
{
	struct Asset;
}

namespace Rx
{
	struct Renderer;

	struct Window
	{
		Window(std::string title, int w, int h, bool resizeable) : width(w), height(h)
		{
			this->sdlWin = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | (resizeable ? SDL_WINDOW_RESIZABLE : 0));

			if(!this->sdlWin) ERROR("Failed to initialise SDL Window! (%dx%d)", w, h);

			LOG("Created new SDL Window with dimensions %dx%d", this->width, this->height);
		}

		~Window() { SDL_DestroyWindow(this->sdlWin); }
		SDL_Window* sdlWin;

		int width;
		int height;
	};



	struct Surface
	{
		Surface(std::string path);
		Surface(AssetLoader::Asset* ass);
		~Surface();

		SDL_Surface* sdlSurf;
		AssetLoader::Asset* asset;


		private:
			Surface(SDL_Surface* sdlSurf);
	};

	struct Texture
	{
		Texture(std::string path, Renderer* rend);
		Texture(AssetLoader::Asset* ass, Renderer* rend);
		Texture(Surface* surf, Renderer* rend);
		~Texture();

		SDL_Texture* sdlTexture;
		uint32_t glTextureID;
		Surface* surf;

		uint64_t width;
		uint64_t height;
	};
}





