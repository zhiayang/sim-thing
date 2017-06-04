// GBuffer.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "rx.h"
#include <glbinding/gl/gl.h>

namespace rx
{
	GBuffer* GBuffer::create(Renderer* r)
	{
		using namespace gl;

		GBuffer* gbuf = new GBuffer();

		int width = r->window->width * r->window->displayScale;
		int height = r->window->height * r->window->displayScale;

		glGenFramebuffers(1, &gbuf->gFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gbuf->gFramebuffer);

		// position buffer
		{
			glGenTextures(1, &gbuf->positionBuffer);
			glBindTexture(GL_TEXTURE_2D, gbuf->positionBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuf->positionBuffer, 0);
		}

		// normal buffer
		{
			glGenTextures(1, &gbuf->normalBuffer);
			glBindTexture(GL_TEXTURE_2D, gbuf->normalBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuf->normalBuffer, 0);
		}

		// color + specular buffer
		{
			glGenTextures(1, &gbuf->colourBuffer);
			glBindTexture(GL_TEXTURE_2D, gbuf->colourBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuf->colourBuffer, 0);
		}


		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
		GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// depth buffer
		{
			glGenRenderbuffers(1, &gbuf->depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, gbuf->depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gbuf->depthBuffer);
		}

		// finally check if framebuffer is complete
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			ERROR("Framebuffer not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return gbuf;
	}
}




