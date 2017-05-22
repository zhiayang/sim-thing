// SDLWrapper.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <assert.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
// #include "SDL2/SDL_opengl.h"

#include <glbinding/gl/enum.h>

#include <glm/mat4x4.hpp>

#include "utilities.h"
#include "assetloader.h"
#include "mathprimitives.h"

struct ImDrawData;

#include "stb_truetype.h"

namespace Rx
{
	// font stuff
	struct Font
	{
		Font() { }
		Font(std::string n) : name(n) { }

		// note: these are shallow things.
		#if 0
		Font(const Font& o)
		{
			this->name				= o.name;
			this->ttfBufferSize		= o.ttfBufferSize;
			this->ttfBuffer			= o.ttfBuffer;
			this->atlas				= o.atlas;
			this->atlasWidth		= o.atlasWidth;
			this->atlasHeight		= o.atlasHeight;
			this->vertOversample	= o.vertOversample;
			this->horzOversample	= o.horzOversample;
			this->firstChar			= o.firstChar;
			this->numChars			= o.numChars;
			this->charInfo			= o.charInfo;
		}

		Font& operator = (const Font& o)
		{
			this->name				= o.name;
			this->ttfBufferSize		= o.ttfBufferSize;
			this->ttfBuffer			= o.ttfBuffer;
			this->atlas				= o.atlas;
			this->atlasWidth		= o.atlasWidth;
			this->atlasHeight		= o.atlasHeight;
			this->vertOversample	= o.vertOversample;
			this->horzOversample	= o.horzOversample;
			this->firstChar			= o.firstChar;
			this->numChars			= o.numChars;
			this->charInfo			= o.charInfo;

			return *this;
		}
		#endif

		Font(const Font&) = delete;
		Font& operator = (const Font&) = delete;

		std::string name;

		uint8_t* ttfBuffer = 0;
		size_t ttfBufferSize = 0;

		uint8_t* atlas = 0;
		size_t atlasWidth = 0;
		size_t atlasHeight = 0;

		size_t vertOversample = 0;
		size_t horzOversample = 0;

		uint32_t firstChar = 0;
		size_t numChars = 0;

		gl::GLuint glTextureID = -1;
		stbtt_packedchar* charInfo = 0;
	};

	struct FontGlyphPos
	{
		glm::vec2 vertices[4];
		glm::vec2 uvs[4];
	};

	Font* getFont(std::string name, size_t pixelSize, uint32_t firstChar, size_t numChars, size_t oversampleH, size_t oversampleV);
	FontGlyphPos getGlyphPosition(Font* font, uint32_t u32);

	void closeAllFonts();


	void SetupDefaultStyle();










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

			RenderVerticesFilled,
			RenderVerticesWireframe,

			RenderText,
		};

		RenderCommand() { }
		RenderCommand(const RenderCommand& o)
		{
			this->type			= o.type;
			this->uvs			= o.uvs;
			this->colours		= o.colours;
			this->normals		= o.normals;
			this->vertices		= o.vertices;
			this->dimensions	= o.dimensions;
			this->textureToBind	= o.textureToBind;
		}

		RenderCommand& operator = (const RenderCommand& o)
		{
			this->type			= o.type;
			this->uvs			= o.uvs;
			this->colours		= o.colours;
			this->normals		= o.normals;
			this->vertices		= o.vertices;
			this->dimensions	= o.dimensions;
			this->textureToBind	= o.textureToBind;

			return *this;
		}

		size_t dimensions = 0;
		bool isInScreenSpace = false;
		gl::GLuint textureToBind = -1;
		CommandType type = CommandType::Invalid;

		std::vector<glm::vec2> uvs;
		std::vector<glm::vec4> colours;

		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> vertices;
	};

	struct Renderer
	{
		Renderer(Window* win, SDL_GLContext glc, util::colour clearColour, glm::mat4 camera, gl::GLuint mainShaderProg,
			gl::GLuint textShaderProg, double fov, double width, double height, double near, double far);

		void clearRenderList();
		void renderAll();

		void setCamera(glm::mat4 cameraViewMatrix);
		glm::mat4 getCameraViewMatrix();

		void setProjectionMatrix(glm::mat4 projMatrix);
		glm::mat4 getProjectionMatrix();

		void setProjectionMatrix(double fieldOfView, double width, double height, double nearPlane, double farPlane);

		double getFOV();
		double getNearPlane();
		double getFarPlane();

		void updateWindowSize(double width, double height);

		void setShaderProgram(gl::GLuint programId);
		gl::GLuint getShaderProgramId();




		// start of 'rendering' functions, so to speak

		void clearScreen(util::colour colour);

		void renderVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals,
			std::vector<glm::vec2> uvs);

		// screenspace takes pixel positions
		// glspace takes 0-1 positions
		void renderStringInScreenSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos);
		void renderStringInGLSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos);




		std::vector<RenderCommand> renderList;

		Window* window;
		SDL_GLContext glContext;

		private:
			glm::mat4 cameraMatrix;
			glm::mat4 projectionMatrix;

			util::colour clearColour;

			double _fov = 0;
			double _near = 0;
			double _far = 0;
			double _width = 0;
			double _height = 0;

			gl::GLuint mainShaderProgram = -1;
			gl::GLuint textShaderProgram = -1;
			gl::GLuint mvpMatrixId = -1;
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








	// imgui stuff

	std::pair<SDL_GLContext, Rx::Window*> Initialise(int width, int height);
	std::pair<SDL_Event, bool> ProcessEvents();

	// rendering stuff.
	void PreFrame(Rx::Renderer* r);
	void BeginFrame(Rx::Renderer* r);
	void EndFrame(Rx::Renderer* r);






	// internal stuff, mostly
	// void SetupOpenGL2D(ImDrawData* draw_data, int* fb_width, int* fb_height);
	// void RenderImGui(ImDrawData* draw_data, int fb_height);
	// void FinishOpenGL2D();


	// void SetupOpenGL3D();
	// void FinishOpenGL3D();





}

















