// shaders.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <glbinding/gl/types.h>

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

		gl::GLuint getUniform(std::string name);

		std::string name;
		gl::GLuint progId = -1;
		std::unordered_map<std::string, gl::GLuint> uniformLocations;
	};
}
