// Texture.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "SDLWrapper.h"
namespace SDL
{
	Texture::Texture(std::string path, Renderer* r) : Texture(AssetLoader::Load(path.c_str()), r)
	{

	}

	Texture::Texture(AssetLoader::Asset* ass, Renderer* r) : Texture(new Surface(ass), r)
	{

	}

	Texture::Texture(Surface* surf, Renderer* r)
	{
		assert(surf);
		assert(r);
		this->surf = surf;

		this->width = this->surf->sdlSurf->w;
		this->height = this->surf->sdlSurf->h;

		// do opengl shit.
		if(USE_OPENGL)
		{
			glGenTextures(1, &this->glTextureID);
			glBindTexture(GL_TEXTURE_2D, this->glTextureID);
			int texmode = 0;
			if(this->surf->sdlSurf->format->BytesPerPixel == 4)
			{
				if(this->surf->sdlSurf->format->Rmask == 0x000000FF)
					texmode = GL_BGRA_EXT;

				else
					texmode = GL_RGBA;
			}
			else
			{
				if(this->surf->sdlSurf->format->Rmask == 0x000000FF)
					texmode = GL_BGR_EXT;

				else
					texmode = GL_RGB;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, texmode, this->surf->sdlSurf->w, this->surf->sdlSurf->h, 0, texmode, GL_UNSIGNED_BYTE, this->surf->sdlSurf->pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			this->sdlTexture = SDL_CreateTextureFromSurface(r->sdlRenderer, this->surf->sdlSurf);
			if(!this->sdlTexture)
				ERROR("Failed to create texture from surface");
		}
	}

	Texture::~Texture()
	{
		if(USE_OPENGL)
		{
			glDeleteTextures(1, &this->glTextureID);
		}
		else
		{
			SDL_DestroyTexture(this->sdlTexture);
		}

		delete this->surf;
	}
}
