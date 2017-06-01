// ShaderProgram.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "utilities.h"
#include "assetloader.h"
#include "rx/shaders.h"

#include "stx/string_view.hpp"
#include <glbinding/gl/gl.h>

using namespace AssetLoader;

#define DUMP_UNIFORM_LOCATIONS 0

namespace rx
{
	static std::vector<stx::string_view> splitString(const char* str, size_t length)
	{
		std::vector<stx::string_view> lines;
		auto view = stx::string_view(str, length);

		while(true)
		{
			size_t ln = view.find("\n");

			if(ln != stx::string_view::npos)
			{
				// fuck windows line endings
				if(view.length() > 1 && view.data()[ln - 1] == '\r')
				{
					// don't +1 because we don't want the \n in the line itself
					lines.push_back(stx::string_view(view.data(), ln - 1));
				}
				else
				{
					// don't +1 because we don't want the \n in the line itself
					lines.push_back(stx::string_view(view.data(), ln));
				}

				// +1 here because we want to remove the \n from the stream.
				view.remove_prefix(ln + 1);
			}
			else
			{
				break;
			}
		}

		return lines;
	}

	static std::string loadShaderSource(std::string path, std::string kind)
	{
		Asset* shaderAsset = AssetLoader::Load(path.c_str());
		auto lines = splitString((const char*) shaderAsset->raw, shaderAsset->length);

		std::vector<std::string> finalLines;
		{
			size_t linenum = 1;
			for(auto line : lines)
			{
				if(line.find("#pragma include ") == 0)
				{
					line.remove_prefix(strlen("#pragma include "));
					while(!line.empty() && line.front() == ' ')
						line.remove_prefix(1);

					if(line.empty() || line[0] != '"')
					{
						ERROR("Syntax error while preprocessing %s shader %s:%zu: malformed include directive, missing opening ' \
							\"' after #include", kind.c_str(), path.c_str(), linenum);
					}

					line.remove_prefix(1);
					if(line.back() != '"')
					{
						ERROR("Syntax error while preprocessing %s shader %s:%zu: malformed include directive, missing closing '\"'",
							kind.c_str(), path.c_str(), linenum);
					}

					std::string incPath = line.to_string();
					incPath.pop_back();

					// load the asset, hopefully.
					Asset* incAsset = AssetLoader::Load(("shaders/" + incPath).c_str());
					auto inclines = splitString((const char*) incAsset->raw, incAsset->length);
					{
						// insert the inc into the vsrc
						for(auto il : inclines)
							finalLines.push_back(il.to_string());
					}
					AssetLoader::Unload(incAsset);
				}
				else
				{
					finalLines.push_back(line.to_string());
				}

				linenum++;
			}
		}
		AssetLoader::Unload(shaderAsset);

		std::string source;
		for(auto l : finalLines)
			source += l + "\n";

		return source;
	}

	static gl::GLuint compileAndLinkGLShaderProgram(ShaderSource source)
	{
		using namespace gl;

		GLuint vShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint progID = glCreateProgram();

		{
			std::string vsrc = loadShaderSource(source.vertexShaderPath, "vertex");
			const char* vstr = vsrc.c_str();

			glShaderSource(vShaderID, 1, &vstr, nullptr);
			glCompileShader(vShaderID);

			GLint res = 0;
			int loglen = 0;

			// Check vertex shader
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
			std::string fsrc = loadShaderSource(source.fragmentShaderPath, "fragment");
			const char* fstr = fsrc.c_str();

			glShaderSource(fShaderID, 1, &fstr, nullptr);
			glCompileShader(fShaderID);

			GLint res = 0;
			int loglen = 0;

			// Check fragment shader
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



	ShaderProgram::ShaderProgram(std::string sname, ShaderSource source)
	{
		using namespace gl;
		this->name = sname;

		this->progId = compileAndLinkGLShaderProgram(source);
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

			if(DUMP_UNIFORM_LOCATIONS)
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

















