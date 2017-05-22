// Shaders.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>

#include <glbinding/gl/gl.h>

#include "utilities.h"
#include "assetloader.h"

namespace AssetLoader
{
	gl::GLuint compileAndLinkGLShaderProgram(std::string vertexPath, std::string fragmentPath)
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
		LOG("Linked shader program with ID %d\n", progID);

		glDetachShader(progID, vShaderID);
		glDetachShader(progID, fShaderID);

		glDeleteShader(vShaderID);
		glDeleteShader(fShaderID);

		return progID;
	}
}







