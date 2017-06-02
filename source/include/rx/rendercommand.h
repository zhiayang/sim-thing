// rendercommand.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <vector>

#include <glbinding/gl/types.h>

#include "lx.h"
#include "rx/model.h"

namespace rx
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

		RenderCommand();

		id_t id;

		bool wireframe = false;

		size_t dimensions = 0;
		bool isInScreenSpace = false;
		gl::GLuint textureToBind = -1;
		CommandType type = CommandType::Invalid;

		Material material;

		std::vector<lx::vec2> uvs;
		std::vector<lx::vec4> colours;

		std::vector<lx::vec3> normals;
		std::vector<lx::vec3> vertices;
	};
}
