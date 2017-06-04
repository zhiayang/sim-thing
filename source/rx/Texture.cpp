// Texture.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>

#include "rx.h"
#include "glwrapper.h"

#include <glbinding/gl/gl.h>

namespace rx
{
	Texture::Texture(std::string path) : Texture(AssetLoader::Load(path.c_str()))
	{

	}

	Texture::Texture(AssetLoader::Asset* ass) : Texture(new Surface(ass))
	{
		this->ownSurface = true;
	}

	Texture::Texture(uint8_t* bytes, size_t width, size_t height, ImageFormat format, bool autotex)
		: Texture(new Surface(bytes, width, height, format), autotex)
	{
		this->ownSurface = true;
	}

	Texture::Texture(Surface* surf, bool autotex)
	{
		using namespace gl;
		assert(surf);

		this->surf = surf;

		this->width = this->surf->width;
		this->height = this->surf->height;

		// do opengl shit.
		if(autotex)
		{
			glGenTextures(1, &this->glTextureID);
			glBindTexture(GL_TEXTURE_2D, this->glTextureID);

			GLenum texmode;
			if(surf->format == ImageFormat::RGBA)
			{
				texmode = GL_RGBA;
			}
			else if(surf->format == ImageFormat::RGB)
			{
				texmode = GL_RGB;
			}
			else if(surf->format == ImageFormat::GREYSCALE)
			{
				texmode = GL_RED;
			}
			else
			{
				texmode = GL_INVALID_ENUM;
				ERROR("Unsupported surface format for texture");
			}

			glTexImage2D(GL_TEXTURE_2D, 0, texmode, this->width, this->height, 0, texmode, GL_UNSIGNED_BYTE, this->surf->data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			this->glTextureID = 0;
		}
	}

	Texture::~Texture()
	{
		gl::glDeleteTextures(1, &this->glTextureID);

		if(this->ownSurface)
			delete this->surf;
	}
}








