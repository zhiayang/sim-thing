// RenderObject.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "rx.h"

#include <vector>
#include <assert.h>
#include <glbinding/gl/gl.h>

namespace rx
{
	using namespace gl;

	RenderObject::RenderObject()
	{
		static id_t __id = 0;
		this->id = __id++;

		glGenVertexArrays(1, &this->vertexArrayObject);
		glBindVertexArray(this->vertexArrayObject);
	}

	RenderObject::~RenderObject()
	{
		glDeleteBuffers(this->buffers.size(), &this->buffers[0]);
		glDeleteVertexArrays(1, &this->vertexArrayObject);
	}

	RenderObject* RenderObject::fromTexturedVertices(const std::vector<lx::fvec3>& verts, const std::vector<lx::fvec2>& uvs,
		const std::vector<lx::fvec3>& normals)
	{
		// leverage the `fromColouredVertices` method
		auto colours = std::vector<lx::fvec4>(verts.size(), util::colour::white());
		auto ret = RenderObject::fromColouredVertices(verts, colours, normals);

		GLuint uvBuffer;
		glGenBuffers(1, &uvBuffer);
		ret->buffers.push_back(uvBuffer);

		glBindVertexArray(ret->vertexArrayObject);

		glEnableVertexAttribArray(3);
		{
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(lx::fvec2), &uvs[0],
				GL_STATIC_DRAW);

			glVertexAttribPointer(
				3,			// location
				2,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*) 0	// array buffer offset
			);
		}

		glBindVertexArray(0);
		return ret;
	}

	RenderObject* RenderObject::fromColouredVertices(const std::vector<lx::fvec3>& verts, const std::vector<lx::fvec4>& colours,
		const std::vector<lx::fvec3>& normals)
	{
		using namespace gl;

		auto ret = new RenderObject();

		GLuint vertBuffer;		glGenBuffers(1, &vertBuffer);
		GLuint colourBuffer;	glGenBuffers(1, &colourBuffer);
		GLuint normalBuffer;	glGenBuffers(1, &normalBuffer);

		ret->buffers = { vertBuffer, colourBuffer, normalBuffer };

		glEnableVertexAttribArray(0);
		{
			// we always have vertices
			glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
			glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(lx::fvec3), &verts[0],
				GL_STATIC_DRAW);

			glVertexAttribPointer(
				0,			// location
				3,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*) 0	// array buffer offset
			);
		}

		glEnableVertexAttribArray(1);
		{
			glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
			glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(lx::fvec4), &colours[0],
				GL_STATIC_DRAW);

			glVertexAttribPointer(
				1,			// location
				4,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*) 0	// array buffer offset
			);
		}

		// if we have normals:
		if(normals.size() > 0)
		{
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(lx::fvec3), &normals[0],
				GL_STATIC_DRAW);

			glVertexAttribPointer(
				2,			// location
				3,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*) 0	// array buffer offset
			);
		}

		glBindVertexArray(0);

		ret->renderType = RenderType::Vertices;
		ret->arrayLength = verts.size();

		return ret;
	}



	std::vector<RenderObject*> RenderObject::fromModel(const Model& model)
	{
		std::vector<RenderObject*> ret;
		for(auto pair : model.objects)
			ret.push_back(RenderObject::fromMesh(pair.first, pair.second));

		return ret;
	}

	RenderObject* RenderObject::fromMesh(const Mesh& mesh, const Material& mat)
	{
		assert(!mesh.faces.empty());

		std::vector<lx::fvec3> vertices;
		std::vector<lx::fvec3> normals;
		std::vector<lx::fvec2> uvs;

		for(auto face : mesh.faces)
		{
			assert(!face.vertices.empty());

			for(auto v : face.vertices)
				vertices.push_back(tof(v));

			for(auto t : face.uvs)
				uvs.push_back(tof(t));

			for(auto n : face.normals)
				normals.push_back(tof(n));
		}

		auto ret = RenderObject::fromTexturedVertices(vertices, uvs, normals);
		ret->material = mat;

		return ret;
	}
}
