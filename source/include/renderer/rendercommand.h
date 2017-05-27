// rendercommand.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glbinding/gl/types.h>

namespace Rx
{
	struct RenderCommand
	{
		enum class CommandType
		{
			Invalid,
			Clear,

			RenderColouredVertices,
			RenderTexturedVertices,

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
			this->wireframe		= o.wireframe;
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
			this->wireframe		= o.wireframe;
			this->dimensions	= o.dimensions;
			this->textureToBind	= o.textureToBind;

			return *this;
		}

		bool wireframe = false;

		size_t dimensions = 0;
		bool isInScreenSpace = false;
		gl::GLuint textureToBind = -1;
		CommandType type = CommandType::Invalid;

		std::vector<glm::vec2> uvs;
		std::vector<glm::vec4> colours;

		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> vertices;
	};
}
