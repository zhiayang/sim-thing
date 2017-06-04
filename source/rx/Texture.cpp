// Texture.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>

#include "rx.h"
#include "glwrapper.h"

#include <glbinding/gl/gl.h>

namespace rx
{
	Texture::Texture(std::string path, Renderer* r) : Texture(AssetLoader::Load(path.c_str()), r)
	{

	}

	Texture::Texture(AssetLoader::Asset* ass, Renderer* r) : Texture(new Surface(ass), r)
	{
		this->ownSurface = true;
	}

	Texture::Texture(Surface* surf, Renderer*)
	{
		using namespace gl;
		assert(surf);

		this->surf = surf;

		this->width = this->surf->width;
		this->height = this->surf->height;
		this->format = this->surf->format;

		// do opengl shit.

		glGenTextures(1, &this->glTextureID);
		GL::pushTextureBinding(this->glTextureID);

		GLenum texmode;
		if(this->format == ImageFormat::RGBA)
		{
			texmode = GL_RGBA;
		}
		else if(this->format == ImageFormat::RGB)
		{
			texmode = GL_RGB;
		}
		else
		{
			texmode = GL_INVALID_ENUM;
			ERROR("Unsupported surface format for texture");
		}

		glTexImage2D(GL_TEXTURE_2D, 0, texmode, this->width, this->height, 0, texmode, GL_UNSIGNED_BYTE, this->surf->data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GL::popTextureBinding();
	}

	Texture::~Texture()
	{
		gl::glDeleteTextures(1, &this->glTextureID);

		if(this->ownSurface)
			delete this->surf;
	}
}








