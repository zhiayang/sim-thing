// ShaderProgram.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "utilities.h"
#include "assetloader.h"
#include "renderer/shaders.h"

#include <glbinding/gl/gl.h>

using namespace AssetLoader;

namespace Rx
{
	static gl::GLuint compileAndLinkGLShaderProgram(std::string vertexPath, std::string fragmentPath)
	{
		using namespace gl;

		GLuint vShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint progID = glCreateProgram();

		{
			Asset* vsAsset = AssetLoader::Load(vertexPath.c_str());
			const char* source = (const char*) vsAsset->raw;
			glShaderSource(vShaderID, 1, &source, nullptr);
			glCompileShader(vShaderID);

			GLint res = 0;
			int loglen = 0;

			// Check Vertex Shader
			glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &res);
			glGetShaderiv(vShaderID, GL_INFO_LOG_LENGTH, &loglen);

			if(loglen > 0)
			{
				char* errmsg = new char[loglen + 1];
				glGetShaderInfoLog(vShaderID, loglen, NULL, errmsg);

				ERROR("Error compiling vertex shader: '%s'", errmsg);
			}
		}


		{
			Asset* fsAsset = AssetLoader::Load(fragmentPath.c_str());
			const char* source = (const char*) fsAsset->raw;
			glShaderSource(fShaderID, 1, &source, nullptr);
			glCompileShader(fShaderID);

			GLint res = 0;
			int loglen = 0;

			// Check Vertex Shader
			glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &res);
			glGetShaderiv(fShaderID, GL_INFO_LOG_LENGTH, &loglen);

			if(loglen > 0)
			{
				char* errmsg = new char[loglen + 1];
				glGetShaderInfoLog(fShaderID, loglen, NULL, errmsg);

				ERROR("Error compiling fragment shader: '%s'", errmsg);
			}
		}


		// Link the program
		{
			glAttachShader(progID, vShaderID);
			glAttachShader(progID, fShaderID);
			glLinkProgram(progID);

			GLint res = 0;
			int loglen = 0;

			// Check the program
			glGetProgramiv(progID, GL_LINK_STATUS, &res);
			glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &loglen);

			if(loglen > 0)
			{
				char* errmsg = new char[loglen + 1];
				glGetProgramInfoLog(progID, loglen, NULL, errmsg);

				ERROR("Error linking program: '%s'", errmsg);
			}
		}
		LOG("Linked shader program with ID %d", progID);

		glDetachShader(progID, vShaderID);
		glDetachShader(progID, fShaderID);

		glDeleteShader(vShaderID);
		glDeleteShader(fShaderID);

		return progID;
	}



	ShaderProgram::ShaderProgram(std::string name) : ShaderProgram(name + ".vs", name + ".fs") { }

	ShaderProgram::ShaderProgram(std::string vertpath, std::string fragpath)
	{
		using namespace gl;
		this->name = vertpath.substr(0, vertpath.find_last_of('.'));

		this->progId = compileAndLinkGLShaderProgram("shaders/" + vertpath, "shaders/" + fragpath);
		assert(this->progId >= 0);

		// enumerate all uniforms in the shader prog, to cache their locations
		GLint activeUniforms = 0;
		glGetProgramiv(this->progId, GL_ACTIVE_UNIFORMS, &activeUniforms);
		this->use();

		for(int unf = 0; unf < activeUniforms; unf++)
		{
			char* namebuf = new char[512];
			GLsizei length = 0;
			glGetActiveUniformName(this->progId, (GLuint) unf, 512, &length, namebuf);

			auto name = std::string(namebuf, length);
			delete[] namebuf;

			GLuint loc = glGetUniformLocation(this->progId, name.c_str());
			this->uniformLocations[name] = loc;

			LOG("Uniform '%s' = %d", name.c_str(), loc);
		}
		LOG("Cached %d uniform locations in shader program '%s'\n", activeUniforms, this->name.c_str());
	}

	gl::GLuint ShaderProgram::getUniform(std::string name)
	{
		if(this->uniformLocations.find(name) == this->uniformLocations.end())
			ERROR("No such uniform named '%s' in shader program '%s'", name.c_str(), this->name.c_str());

		return this->uniformLocations[name];
	}

	void ShaderProgram::use()
	{
		gl::glUseProgram(this->progId);
	}
}


















