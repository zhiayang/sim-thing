// misc.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>

#include <glbinding/gl/types.h>

#include "utilities.h"


namespace AssetLoader
{
	struct Asset;
}

struct SDL_Window;

namespace Rx
{
	struct Renderer;

	struct Window
	{
		Window(std::string title, int w, int h, bool resizeable);
		~Window();
		SDL_Window* sdlWin;

		int width;
		int height;
	};



	enum class ImageFormat
	{
		Invalid,
		RGB,
		RGBA
	};

	struct Surface
	{
		Surface(std::string path);
		Surface(AssetLoader::Asset* ass);
		~Surface();

		uint8_t* data = 0;
		size_t width = 0;
		size_t height = 0;

		ImageFormat format = ImageFormat::Invalid;
	};

	struct Texture
	{
		Texture(Surface* surf, Renderer* rend);
		Texture(std::string path, Renderer* rend);
		Texture(AssetLoader::Asset* ass, Renderer* rend);
		~Texture();

		gl::GLuint glTextureID;

		bool ownSurface = false;
		Surface* surf = 0;

		uint64_t width = 0;
		uint64_t height = 0;

		ImageFormat format = ImageFormat::Invalid;
	};
}





