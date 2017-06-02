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
#include <glm/trigonometric.hpp>

#include "utilities.h"
#include "assetloader.h"

// subheaders
#include "rx/misc.h"
#include "rx/fonts.h"
#include "rx/shaders.h"
#include "rx/lighting.h"
#include "rx/rendercommand.h"


// fuck this shit.
namespace glm
{
	template <typename T>
	T normalise(const T& thing)
	{
		return glm::normalize(thing);
	}
}

namespace rx
{
	struct Model;
	struct Mesh;
	struct Material;

	enum class TextAlignment
	{
		Invalid,
		LeftAligned,
		RightAligned
	};

	struct Camera
	{
		glm::vec3 position;

		glm::vec3 front() const;
		glm::vec3 right() const;
		glm::vec3 up() const;

		float pitch = 0;
		float roll = 0;
		float yaw = 0;
	};

	struct Renderer
	{
		Renderer(Window* win, util::colour clearColour, Camera camera, ShaderProgram textureShaderProg,
			ShaderProgram colourShaderProg, ShaderProgram textShaderProg, double fov, double near, double far);

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
		void addSpotLight(SpotLight light);


		std::vector<PointLight> sortAndUpdatePointLights(glm::vec3 reference);
		std::vector<SpotLight> sortAndUpdateSpotLights(glm::vec3 reference);


		// start of 'rendering' functions, so to speak

		void clearScreen(util::colour colour);

		void renderTexturedVertices(std::vector<glm::vec3> verts, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);
		void renderColouredVertices(std::vector<glm::vec3> verts, std::vector<glm::vec4> colours, std::vector<glm::vec3> normals);

		// screenspace takes pixel positions
		// normalisedscreenspace takes 0-1 positions
		void renderStringInScreenSpace(std::string txt, rx::Font* font, float size, glm::vec2 pos,
			TextAlignment align = TextAlignment::LeftAligned);

		void renderStringInNormalisedScreenSpace(std::string txt, rx::Font* font, float size, glm::vec2 pos,
			TextAlignment align = TextAlignment::LeftAligned);

		void renderMesh(const Mesh& mesh, const Material& mat, glm::mat4 transform);
		void renderModel(const Model& model, glm::mat4 transform);

		void updateCamera(const Camera& cam);
		Camera getCamera();






		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;


		Window* window = 0;

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

			Texture* placeholderTexture = 0;

			ShaderProgram textureShaderProgram;
			ShaderProgram colourShaderProgram;
			ShaderProgram textShaderProgram;

			std::vector<RenderCommand> renderList;
	};

	// rendering stuff.
	void PreFrame(rx::Renderer* r);
	void BeginFrame(rx::Renderer* r);
	void EndFrame(rx::Renderer* r);
}











