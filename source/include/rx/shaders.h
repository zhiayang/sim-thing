// shaders.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <glbinding/gl/types.h>

#include <glm/fwd.hpp>

namespace rx
{
	struct ShaderSource
	{
		// this is just a reference for us now
		std::string glslVersion;

		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};

	struct ShaderProgram
	{
		ShaderProgram(std::string shaderName, ShaderSource source);

		void use();
		bool isInUse();

		gl::GLuint getUniformLocation(std::string name);

		void setUniform(std::string name, glm::vec4 v4);
		void setUniform(std::string name, glm::vec3 v3);
		void setUniform(std::string name, glm::vec2 v2);

		void setUniform(std::string name, glm::mat4 m4);
		void setUniform(std::string name, glm::mat3 m3);
		void setUniform(std::string name, glm::mat2 m2);

		void setUniform(std::string name, float f);
		void setUniform(std::string name, int i);

		std::string name;
		gl::GLuint progId = -1;
		std::unordered_map<std::string, gl::GLuint> uniformLocations;
	};
}
