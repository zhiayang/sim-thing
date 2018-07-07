// shaders.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <glbinding/gl/types.h>

#include "lx.h"

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
		ShaderProgram(const std::string& shaderName, const ShaderSource& source);

		void use();
		bool isInUse();

		gl::GLuint getUniformLocation(const std::string& name);

		void setUniform(const std::string& name, const lx::fvec4& v4);
		void setUniform(const std::string& name, const lx::fvec3& v3);
		void setUniform(const std::string& name, const lx::fvec2& v2);

		void setUniform(const std::string& name, const lx::fmat4& m4);
		void setUniform(const std::string& name, const lx::fmat3& m3);
		void setUniform(const std::string& name, const lx::fmat2& m2);

		void setUniform(const std::string& name, float f);
		void setUniform(const std::string& name, int i);

		std::string name;
		gl::GLuint progId = -1;
		std::unordered_map<std::string, gl::GLuint> uniformLocations;
	};
}
