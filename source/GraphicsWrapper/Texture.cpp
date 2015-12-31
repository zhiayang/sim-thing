// Texture.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "imgui.h"
#include "glwrapper.h"
#include "graphicswrapper.h"

namespace Rx
{
	Texture::Texture(std::string path, Renderer* r) : Texture(AssetLoader::Load(path.c_str()), r)
	{

	}

	Texture::Texture(AssetLoader::Asset* ass, Renderer* r) : Texture(new Surface(ass), r)
	{

	}

	Texture::Texture(Surface* surf, Renderer*)
	{
		assert(surf);

		this->surf = surf;

		this->width = this->surf->sdlSurf->w;
		this->height = this->surf->sdlSurf->h;

		// do opengl shit.

		glGenTextures(1, &this->glTextureID);
		GL::pushTextureBinding(this->glTextureID);

		int texmode = 0;
		if(this->surf->sdlSurf->format->BytesPerPixel == 4)
		{
			texmode = GL_RGBA;
		}
		else
		{
			texmode = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, texmode, this->width, this->height, 0, texmode, GL_UNSIGNED_BYTE, this->surf->sdlSurf->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GL::popTextureBinding();


		// this is another one of those "retina" hacks.
		if(ImGui::GetIO().DisplayFramebufferScale.x != 1.0 || ImGui::GetIO().DisplayFramebufferScale.y != 1.0)
		{
			// then we report our size as half our actual size.
			this->width /= 2;
			this->height /= 2;
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &this->glTextureID);
		delete this->surf;
	}
}








