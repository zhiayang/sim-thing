// OpenGL.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>
#include <deque>

#include "imgui.h"

#include "glwrapper.h"
#include "graphicswrapper.h"

#include <glbinding/gl/gl.h>

namespace GL
{
	static std::deque<int> glTextureStack;

	void pushTextureBinding(int id)
	{
		int cur = -1;

		if(glTextureStack.size() > 0)
			cur = glTextureStack.back();

		glTextureStack.push_back(id);

		if(cur == -1 || cur != id)
			gl::glBindTexture(gl::GL_TEXTURE_2D, id);
	}

	int popTextureBinding()
	{
		assert(glTextureStack.size() > 0 && "mismatched push/pop");

		int cur = glTextureStack.back();
		glTextureStack.pop_back();

		if(glTextureStack.size() > 0 && cur != glTextureStack.back())
			gl::glBindTexture(gl::GL_TEXTURE_2D, glTextureStack.back());

		else
			gl::glBindTexture(gl::GL_TEXTURE_2D, 0);

		return cur;
	}
}





























