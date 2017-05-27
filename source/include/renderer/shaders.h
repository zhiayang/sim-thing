// shaders.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>
#include <unordered_map>

#include <glbinding/gl/types.h>

namespace Rx
{
	struct ShaderProgram
	{
		ShaderProgram(std::string name);
		ShaderProgram(std::string vertShaderPath, std::string fragShaderPath);

		void use();

		gl::GLuint getUniform(std::string name);

		std::string name;
		gl::GLuint progId = -1;
		std::unordered_map<std::string, gl::GLuint> uniformLocations;
	};
}
