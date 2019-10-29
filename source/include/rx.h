// rx.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

#include "lx.h"
#include "utilities.h"
#include "assetloader.h"

// subheaders
#include "rx/misc.h"
#include "rx/fonts.h"
#include "rx/model.h"
#include "rx/shaders.h"
#include "rx/lighting.h"

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
		lx::vec3 position;

		lx::vec3 front() const;
		lx::vec3 right() const;
		lx::vec3 up() const;

		float pitch = 0;
		float roll = 0;
		float yaw = 0;
	};



	enum class RenderType
	{
		Invalid,

		Text,
		Vertices,
	};

	struct RenderObject
	{
		RenderObject();
		~RenderObject();

		id_t id;

		Material material;
		bool wireframe = false;

		size_t arrayLength = 0;
		gl::GLuint vertexArrayObject = -1;

		RenderType renderType = RenderType::Invalid;
		size_t shaderProgramIndex = 0;

		std::vector<gl::GLuint> buffers;

		static RenderObject* fromTexturedVertices(const std::vector<lx::fvec3>& verts, const std::vector<lx::fvec2>& uvs,
			const std::vector<lx::fvec3>& normals);

		static RenderObject* fromColouredVertices(const std::vector<lx::fvec3>& verts, const std::vector<lx::fvec4>& colours,
			const std::vector<lx::fvec3>& normals);

		static std::vector<RenderObject*> fromModel(const Model& model);
		static RenderObject* fromMesh(const Mesh& mesh, const Material& mat);
	};

	struct RenderCommand
	{
		RenderCommand();

		id_t id;

		lx::mat4 modelMatrix;
		RenderObject* renderObject = 0;

		// fields below are used specifically for text rendering.
		float textScale = 0;
		std::vector<size_t> indices;
		std::vector<lx::fvec2> positions;
		util::colour textColour = util::colour::white();
	};

	struct GBuffer
	{
		gl::GLuint gFramebuffer;

		gl::GLuint depthBuffer;
		gl::GLuint normalBuffer;
		gl::GLuint diffuseBuffer;
		gl::GLuint specularBuffer;
		gl::GLuint positionBuffer;

		static GBuffer* create(Renderer* renderer);

		private:
			GBuffer() { }
	};


	struct ShaderPipeline
	{
		ShaderProgram textShader;
		std::vector<ShaderProgram> shaders;
	};

	struct Renderer
	{
		Renderer(Window* win, util::colour clearColour, const Camera& camera, const ShaderPipeline& shaderPipeline,
			double fov, double near, double far);

		void clearRenderList();

		void renderForward();

		void setCamera(lx::mat4 cameraViewMatrix);
		lx::mat4 getCameraViewMatrix();

		void setProjectionMatrix(lx::mat4 projMatrix);
		lx::mat4 getProjectionMatrix();

		double getFOV();
		double getNearPlane();
		double getFarPlane();

		void updateWindowSize(double width, double height);


		void setAmbientLighting(const util::colour& colour, float intensity);
		PointLight& addPointLight(const PointLight& light);
		SpotLight& addSpotLight(const SpotLight& light);


		std::vector<PointLight> sortAndUpdatePointLights(const lx::fvec3& reference);
		std::vector<SpotLight> sortAndUpdateSpotLights(const lx::fvec3& reference);

		void renderObject(RenderObject* ro, const lx::mat4& transform);

		// screenspace takes pixel positions
		// normalisedscreenspace takes 0-1 positions
		void renderStringInScreenSpace(const std::string& txt, rx::Font* font, float size, const lx::fvec2& pos,
			const util::colour& colour, TextAlignment align = TextAlignment::LeftAligned);

		void renderStringInNormalisedScreenSpace(const std::string& txt, rx::Font* font, float size, const lx::fvec2& pos,
			const util::colour& colour, TextAlignment align = TextAlignment::LeftAligned);

		// void renderMesh(const Mesh& mesh, const Material& mat, lx::mat4 transform);
		// void renderModel(const Model& model, lx::mat4 transform);

		void updateCamera(const Camera& cam);
		Camera getCamera();



		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;


		Window* window = 0;

		private:
			lx::mat4 cameraMatrix;
			lx::mat4 projectionMatrix;

			Camera camera;
			util::colour clearColour;

			double _fov = 0;
			double _near = 0;
			double _far = 0;
			double _width = 0;
			double _height = 0;
			double _resolutionScale = 0;

			GBuffer* gBuffer = 0;

			Texture* placeholderTexture = 0;

			ShaderPipeline pipeline;
			std::vector<RenderCommand> forwardList;


		friend void PreFrame(rx::Renderer* r);
		friend void BeginFrame(rx::Renderer* r);
		friend void EndFrame(rx::Renderer* r);
	};

	// rendering stuff.
	void PreFrame(rx::Renderer* r);
	void BeginFrame(rx::Renderer* r);
	void EndFrame(rx::Renderer* r);
}











