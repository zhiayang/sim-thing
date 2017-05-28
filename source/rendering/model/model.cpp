// model.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "utilities.h"

#include "renderer/rx.h"
#include "renderer/model.h"

#include <glm/glm.hpp>

#include <sstream>

#include <stx/string_view.hpp>

using namespace AssetLoader;


namespace Rx
{
	Mesh::Mesh()
	{
		static id_t __id = 1;
		this->id = __id++;
	}

	Model::Model()
	{
		static id_t __id = 1;
		this->id = __id++;
	}

	void Model::addMesh(const Mesh& mesh, const Material& mat)
	{
		this->objects.push_back(std::make_pair(mesh, mat));
	}

	Model Model::fromMesh(Mesh mesh, Material mat)
	{
		Model model;
		model.addMesh(mesh, mat);

		return model;
	}

	Model loadModelFromAsset(Asset* asset, double scale)
	{
		switch(asset->type)
		{
			case AssetType::ModelOBJ:
				return loadModelFromWavefrontOBJ(asset, scale);

			default:
				ERROR("Invalid asset to load model from (type %d)", asset->type);
		}
	}




	Face triangulateQuadFace(Face face)
	{
		std::vector<glm::vec3> verts;
		std::vector<glm::vec3> norms;
		std::vector<glm::vec2> uvs;

		// only supprot quads
		assert(face.vertices.size() == 4 && "only quads supported");
		auto a = face.vertices[0];
		auto b = face.vertices[1];
		auto c = face.vertices[2];
		auto d = face.vertices[3];

		auto an = face.normals.size() > 0 ? face.normals[0] : glm::vec3();
		auto bn = face.normals.size() > 0 ? face.normals[1] : glm::vec3();
		auto cn = face.normals.size() > 0 ? face.normals[2] : glm::vec3();
		auto dn = face.normals.size() > 0 ? face.normals[3] : glm::vec3();

		auto at = face.uvs.size() > 0 ? face.uvs[0] : glm::vec2();
		auto bt = face.uvs.size() > 0 ? face.uvs[1] : glm::vec2();
		auto ct = face.uvs.size() > 0 ? face.uvs[2] : glm::vec2();
		auto dt = face.uvs.size() > 0 ? face.uvs[3] : glm::vec2();

		auto ac = glm::distance(a, c);
		auto bd = glm::distance(b, d);

		if(ac < bd)
		{
			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(an);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(an);

			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(an);
			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(an);
		}
		else
		{
			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(bn);
			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(dn);

			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(dn);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(bn);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(cn);
		}

		Face ret;

		ret.vertices = verts;
		ret.normals = norms;
		ret.uvs = uvs;

		ret.faceNormal = calculateNormalForFace(ret);

		return ret;
	}



	glm::vec3 calculateNormalForFace(Face face)
	{
		// note: there's an implicit understanding here. if we're given a quad face, then the normal of
		// that quad face should equal the normal of the 2 triangles, since they must lie in the same plane.

		if(face.vertices.size() == 4)
			face = triangulateQuadFace(face);

		auto a = face.vertices[0];
		auto b = face.vertices[1];
		auto c = face.vertices[2];

		return glm::normalise(glm::cross(b - a, c - a));
	}




	Mesh Mesh::getUnitCube()
	{
		Mesh ret;
		ret.name = "unit_cube";

		Face top;
		{
			top.vertices.push_back(glm::vec3(0.5, 0.5, -0.5));		top.uvs.push_back(glm::vec2(0.0, 1.0));
			top.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5));		top.uvs.push_back(glm::vec2(1.0, 1.0));
			top.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5));		top.uvs.push_back(glm::vec2(1.0, 0.0));
			top.vertices.push_back(glm::vec3(0.5, 0.5, 0.5));		top.uvs.push_back(glm::vec2(0.0, 0.0));
		}

		Face bottom;
		{
			bottom.vertices.push_back(glm::vec3(0.5, -0.5, 0.5));	bottom.uvs.push_back(glm::vec2(0.0, 0.0));
			bottom.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5));	bottom.uvs.push_back(glm::vec2(1.0, 0.0));
			bottom.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5));	bottom.uvs.push_back(glm::vec2(1.0, 1.0));
			bottom.vertices.push_back(glm::vec3(0.5, -0.5, -0.5));	bottom.uvs.push_back(glm::vec2(0.0, 1.0));
		}

		Face left;
		{
			left.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5));	left.uvs.push_back(glm::vec2(1.0, 0.0));
			left.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5));	left.uvs.push_back(glm::vec2(1.0, 1.0));
			left.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5));	left.uvs.push_back(glm::vec2(0.0, 1.0));
			left.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5));		left.uvs.push_back(glm::vec2(0.0, 0.0));
		}

		Face right;
		{
			right.vertices.push_back(glm::vec3(0.5, 0.5, -0.5));	right.uvs.push_back(glm::vec2(0.0, 0.0));
			right.vertices.push_back(glm::vec3(0.5, 0.5, 0.5));		right.uvs.push_back(glm::vec2(1.0, 0.0));
			right.vertices.push_back(glm::vec3(0.5, -0.5, 0.5));	right.uvs.push_back(glm::vec2(1.0, 1.0));
			right.vertices.push_back(glm::vec3(0.5, -0.5, -0.5));	right.uvs.push_back(glm::vec2(0.0, 1.0));
		}

		Face front;
		{
			front.vertices.push_back(glm::vec3(0.5, 0.5, 0.5));		front.uvs.push_back(glm::vec2(0.0, 0.0));
			front.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5));	front.uvs.push_back(glm::vec2(1.0, 0.0));
			front.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5));	front.uvs.push_back(glm::vec2(1.0, 1.0));
			front.vertices.push_back(glm::vec3(0.5, -0.5, 0.5));	front.uvs.push_back(glm::vec2(0.0, 1.0));
		}

		Face back;
		{
			back.vertices.push_back(glm::vec3(0.5, 0.5, -0.5));		back.uvs.push_back(glm::vec2(0.0, 0.0));
			back.vertices.push_back(glm::vec3(0.5, -0.5, -0.5));	back.uvs.push_back(glm::vec2(0.0, 1.0));
			back.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5));	back.uvs.push_back(glm::vec2(1.0, 1.0));
			back.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5));	back.uvs.push_back(glm::vec2(1.0, 0.0));
		}

		ret.faces.push_back(top);
		ret.faces.push_back(bottom);
		ret.faces.push_back(left);
		ret.faces.push_back(right);
		ret.faces.push_back(front);
		ret.faces.push_back(back);

		for(auto& face : ret.faces)
		{
			face = triangulateQuadFace(face);
			face.faceNormal = calculateNormalForFace(face);

			// LOG("normal = (%f, %f, %f)\n", face.faceNormal.x, face.faceNormal.y, face.faceNormal.z);

			// insert what is basically 6 copies of the same normal, since it's a cube.
			face.normals.clear();
			face.normals.insert(face.normals.begin(), face.vertices.size(), face.faceNormal);
		}

		return ret;
	}
}




















