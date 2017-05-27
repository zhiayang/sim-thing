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

#include "utilities.h"
#include "assetloader.h"

// subheaders
#include "renderer/misc.h"
#include "renderer/fonts.h"
#include "renderer/shaders.h"
#include "renderer/lighting.h"
#include "renderer/rendercommand.h"

namespace Rx
{
	struct Model;
	enum class TextAlignment
	{
		Invalid,
		LeftAligned,
		RightAligned
	};

	struct Camera
	{
		glm::vec3 position;
		glm::vec3 lookingAt;
		glm::vec3 rotation;
	};

	struct Renderer
	{
		Renderer(Window* win, SDL_GLContext glc, util::colour clearColour, Camera camera, ShaderProgram textureShaderProg,
			ShaderProgram colourShaderProg, ShaderProgram textShaderProg, double fov, double width, double height, double resscale,
			double near, double far);

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



		void setAmbientLighting(glm::vec4 colour, float intensity);
		void addPointLight(PointLight light);



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

		void renderModel(Model* model, glm::mat4 transform, glm::vec4 col);


		void setCamera(Camera cam);
		Camera getCamera();






		std::vector<PointLight> pointLights;


		Window* window = 0;
		SDL_GLContext glContext;

		private:
			glm::mat4 cameraMatrix;
			glm::mat4 projectionMatrix;

			Camera camera;
			util::colour clearColour;

			double _fov = 0;
			double _near = 0;
			double _far = 0;
			double _width = 0;
			double _height = 0;
			double _resolutionScale = 0;

			ShaderProgram textureShaderProgram;
			ShaderProgram colourShaderProgram;
			ShaderProgram textShaderProgram;

			gl::GLuint modelMatrixId_colourShader = -1;
			gl::GLuint viewMatrixId_colourShader = -1;
			gl::GLuint projMatrixId_colourShader = -1;

			gl::GLuint modelMatrixId_textureShader = -1;
			gl::GLuint viewMatrixId_textureShader = -1;
			gl::GLuint projMatrixId_textureShader = -1;

			gl::GLuint orthoProjectionMatrixId = -1;


			std::vector<RenderCommand> renderList;
	};





	std::pair<SDL_GLContext, Rx::Window*> Initialise(int width, int height);
	std::pair<SDL_Event, bool> ProcessEvents();

	// rendering stuff.
	void PreFrame(Rx::Renderer* r);
	void BeginFrame(Rx::Renderer* r);
	void EndFrame(Rx::Renderer* r);
}











