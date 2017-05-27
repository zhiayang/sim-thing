// rx.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once


#include <stdint.h>
#include <stddef.h>

#include <string>
#include <unordered_map>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

#include <glm/mat4x4.hpp>

#include "SDL2/SDL.h"

#include "utilities.h"
#include "assetloader.h"
#include "renderer/font.h"

namespace Rx
{
	enum class TextAlignment
	{
		Invalid,
		LeftAligned,
		RightAligned
	};

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

			RenderColouredVerticesFilled,
			RenderColouredVerticesWireframe,

			RenderTexturedVerticesFilled,
			RenderTexturedVerticesWireframe,

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
		Renderer(Window* win, SDL_GLContext glc, util::colour clearColour, glm::mat4 camera, gl::GLuint textureShaderProg,
			gl::GLuint colourShaderProg, gl::GLuint textShaderProg, double fov, double width, double height, double resscale, double near, double far);

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

		void renderTexturedVertices(std::vector<glm::vec3> verts, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);
		void renderColouredVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals);

		// screenspace takes pixel positions
		// normalisedscreenspace takes 0-1 positions
		void renderStringInScreenSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos,
			TextAlignment align = TextAlignment::LeftAligned);

		void renderStringInNormalisedScreenSpace(std::string txt, Rx::Font* font, float size, glm::vec2 pos,
			TextAlignment align = TextAlignment::LeftAligned);





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
			double _resolutionScale = 0;

			gl::GLuint textureShaderProgram = -1;
			gl::GLuint colourShaderProgram = -1;
			gl::GLuint textShaderProgram = -1;

			gl::GLuint mvpMatrixId_textureShader = -1;
			gl::GLuint mvpMatrixId_colourShader = -1;

			gl::GLuint orthoProjectionMatrixId = -1;
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



}
