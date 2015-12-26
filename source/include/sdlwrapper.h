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


	struct RenderCommand
	{
		enum class CommandType
		{
			Invalid,
			Clear,
			RenderTex,
			RenderVerts,
			RenderString,
		};

		RenderCommand() { }

		RenderCommand(const RenderCommand& o) : fill(o.fill), colour(o.colour), texture(o.texture),
			vertices(o.vertices), type(o.type), str(o.str), font(o.font), mode(o.mode) { }

		RenderCommand& operator = (const RenderCommand& o)
		{
			this->fill = o.fill;
			this->colour = o.colour;
			this->texture = o.texture;
			this->vertices = o.vertices;
			this->type = o.type;
			this->str = o.str;
			this->font = o.font;
			this->mode = o.mode;

			return *this;
		}

		void doRender();

		static RenderCommand createRenderString(std::string s, Util::Font font, Util::Colour col, Math::Vector2 pos);
		static RenderCommand createRenderTexture(Texture* tex, Math::Rectangle src, Math::Rectangle dest);
		static RenderCommand createRenderVertices(std::vector<Math::Vector2> vertices, GLenum mode, Util::Colour col, bool fill);

		bool fill = 0;
		Util::Colour colour;
		Texture* texture = 0;
		std::vector<Math::Vector2> vertices;
		CommandType type = CommandType::Invalid;

		std::string str;
		Util::Font font;

		GLenum mode;
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
		void RenderAll();

		// primitive shapes
		void RenderPoint(Math::Vector2 pt);
		void RenderCircle(Math::Circle circ, bool fill = true);
		void RenderRect(Math::Rectangle rect, bool fill = true);
		void RenderLine(Math::Vector2 start, Math::Vector2 end);

		// textures
		void RenderTex(Texture* text, Math::Vector2 at);
		void RenderTex(Texture* text, uint32_t x, uint32_t y);
		void RenderTex(Texture* text, Math::Rectangle dest);
		void RenderTex(Texture* text, Math::Rectangle src, Math::Rectangle dest);


		void SetColour(Util::Colour c);
		Util::Colour GetColour() { return this->drawColour; }

		void SetClearColour(Util::Colour c);
		Util::Colour GetClearColour() { return this->clearColour; }





		// text
		void RenderString(std::string txt, Util::Font font, Math::Vector2 pt);



		std::vector<RenderCommand> renderList;

		Util::Colour drawColour;
		Util::Colour clearColour;

		Window* window;
		SDL_GLContext glContext;


	};

	struct Surface
	{
		Surface(std::string path);
		Surface(AssetLoader::Asset* ass);
		~Surface();

		static Surface* fromText(Util::Font font, Util::Colour colour, std::string txt);

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













	void Initialise();
	void Initialise(uint32_t subs);
}














