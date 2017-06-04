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

namespace rx
{
	struct Renderer;

	struct Window
	{
		Window(std::string n, void* pd, void* pw, int w, int h, int scale)
			: title(n), platformData(pd), platformWindow(pw), width(w), height(h), displayScale(scale) { }
		~Window();

		std::string title;

		void* platformData = 0;
		void* platformWindow = 0;

		int width = 0;
		int height = 0;
		int displayScale = 0;
	};



	enum class ImageFormat
	{
		Invalid,

		GREYSCALE,

		RGB,
		RGBA
	};

	struct Surface
	{
		Surface(std::string path);
		Surface(AssetLoader::Asset* ass);
		Surface(uint8_t* bytes, size_t width, size_t height, ImageFormat format);
		~Surface();

		uint8_t* data = 0;
		size_t length = 0;

		size_t width = 0;
		size_t height = 0;

		ImageFormat format = ImageFormat::Invalid;
	};

	struct Texture
	{
		Texture(Surface* surf, bool autoGenGLTexture = true);
		Texture(std::string path);
		Texture(AssetLoader::Asset* ass);
		Texture(uint8_t* bytes, size_t width, size_t height, ImageFormat format, bool autoGenGLTexture = true);
		~Texture();

		gl::GLuint glTextureID;

		bool ownSurface = false;
		Surface* surf = 0;

		uint64_t width = 0;
		uint64_t height = 0;
	};
}





