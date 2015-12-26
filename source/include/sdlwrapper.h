// SDLWrapper.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <assert.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_opengl.h"

#include "utilities.h"
#include "assetloader.h"
#include "mathprimitives.h"


namespace SDL
{
	struct Texture;
	struct Font;

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

	struct Renderer
	{
		Renderer(Window* win, SDL_GLContext glc, Util::Colour clear)
		{
			assert(win);
			this->window = win;
			this->glContext = glc;
			this->clearColour = clear;

			LOG("Created new SDL Renderer");
		}

		void Clear();

		// primitive shapes
		void RenderPoint(Math::Vector2 pt);
		void RenderCircle(Math::Circle circ, bool fill = true);
		void RenderRect(Math::Rectangle rect, bool fill = true);
		void RenderLine(Math::Vector2 start, Math::Vector2 end);
		void RenderEqTriangle(Math::Vector2 centre, double side);

		// textures
		void Render(Texture* text, Math::Vector2 at);
		void Render(Texture* text, uint32_t x, uint32_t y);
		void Render(Texture* text, Math::Rectangle dest);
		void Render(Texture* text, Math::Rectangle src, Math::Rectangle dest);

		// text
		void RenderText(std::string txt, Font* font, Math::Vector2 pt);

		void SetColour(Util::Colour c);
		Util::Colour GetColour() { return this->drawColour; }

		void SetClearColour(Util::Colour c);
		Util::Colour GetClearColour() { return this->clearColour; }

		Util::Colour drawColour;
		Util::Colour clearColour;


		Window* window;

		SDL_GLContext glContext;


		private:
			void updateGlColour();
	};

	struct Surface
	{
		Surface(std::string path);
		Surface(AssetLoader::Asset* ass);
		Surface(Font* font, std::string txt, Util::Colour c);
		~Surface();

		SDL_Surface* sdlSurf;
		AssetLoader::Asset* asset;
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













	void Initialise();
	void Initialise(uint32_t subs);
}














