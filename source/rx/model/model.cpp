// model.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <sstream>
#include <assert.h>

#include "utilities.h"

#include "rx.h"
#include "rx/model.h"

#include <stx/string_view.hpp>

using namespace AssetLoader;


namespace rx
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
		std::vector<lx::vec3> verts;
		std::vector<lx::vec3> norms;
		std::vector<lx::vec2> uvs;

		// only supprot quads
		assert(face.vertices.size() == 4 && "only quads supported");
		auto a = face.vertices[0];
		auto b = face.vertices[1];
		auto c = face.vertices[2];
		auto d = face.vertices[3];

		auto an = face.normals.size() > 0 ? face.normals[0] : lx::vec3();
		auto bn = face.normals.size() > 0 ? face.normals[1] : lx::vec3();
		auto cn = face.normals.size() > 0 ? face.normals[2] : lx::vec3();
		auto dn = face.normals.size() > 0 ? face.normals[3] : lx::vec3();

		auto at = face.uvs.size() > 0 ? face.uvs[0] : lx::vec2();
		auto bt = face.uvs.size() > 0 ? face.uvs[1] : lx::vec2();
		auto ct = face.uvs.size() > 0 ? face.uvs[2] : lx::vec2();
		auto dt = face.uvs.size() > 0 ? face.uvs[3] : lx::vec2();

		auto ac = lx::distance(a, c);
		auto bd = lx::distance(b, d);

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



	lx::vec3 calculateNormalForFace(Face face)
	{
		// note: there's an implicit understanding here. if we're given a quad face, then the normal of
		// that quad face should equal the normal of the 2 triangles, since they must lie in the same plane.

		if(face.vertices.size() == 4)
			face = triangulateQuadFace(face);

		auto a = face.vertices[0];
		auto b = face.vertices[1];
		auto c = face.vertices[2];

		return lx::cross(b - a, c - a).normalised();
	}




	Mesh Mesh::getUnitCube(float scale)
	{
		Mesh ret;
		ret.name = "unit_cube";

		float p = 0.5 * scale;
		float n = -0.5 * scale;

		Face top;
		{
			top.vertices.push_back(lx::vec3(p, p, n));		top.uvs.push_back(lx::vec2(0.0, 1.0));
			top.vertices.push_back(lx::vec3(n, p, n));		top.uvs.push_back(lx::vec2(1.0, 1.0));
			top.vertices.push_back(lx::vec3(n, p, p));		top.uvs.push_back(lx::vec2(1.0, 0.0));
			top.vertices.push_back(lx::vec3(p, p, p));		top.uvs.push_back(lx::vec2(0.0, 0.0));
		}

		Face bottom;
		{
			bottom.vertices.push_back(lx::vec3(p, n, p));	bottom.uvs.push_back(lx::vec2(0.0, 0.0));
			bottom.vertices.push_back(lx::vec3(n, n, p));	bottom.uvs.push_back(lx::vec2(1.0, 0.0));
			bottom.vertices.push_back(lx::vec3(n, n, n));	bottom.uvs.push_back(lx::vec2(1.0, 1.0));
			bottom.vertices.push_back(lx::vec3(p, n, n));	bottom.uvs.push_back(lx::vec2(0.0, 1.0));
		}

		Face left;
		{
			left.vertices.push_back(lx::vec3(n, p, n));	left.uvs.push_back(lx::vec2(1.0, 0.0));
			left.vertices.push_back(lx::vec3(n, n, n));	left.uvs.push_back(lx::vec2(1.0, 1.0));
			left.vertices.push_back(lx::vec3(n, n, p));	left.uvs.push_back(lx::vec2(0.0, 1.0));
			left.vertices.push_back(lx::vec3(n, p, p));		left.uvs.push_back(lx::vec2(0.0, 0.0));
		}

		Face right;
		{
			right.vertices.push_back(lx::vec3(p, p, n));	right.uvs.push_back(lx::vec2(0.0, 0.0));
			right.vertices.push_back(lx::vec3(p, p, p));		right.uvs.push_back(lx::vec2(1.0, 0.0));
			right.vertices.push_back(lx::vec3(p, n, p));	right.uvs.push_back(lx::vec2(1.0, 1.0));
			right.vertices.push_back(lx::vec3(p, n, n));	right.uvs.push_back(lx::vec2(0.0, 1.0));
		}

		Face front;
		{
			front.vertices.push_back(lx::vec3(p, p, p));		front.uvs.push_back(lx::vec2(0.0, 0.0));
			front.vertices.push_back(lx::vec3(n, p, p));	front.uvs.push_back(lx::vec2(1.0, 0.0));
			front.vertices.push_back(lx::vec3(n, n, p));	front.uvs.push_back(lx::vec2(1.0, 1.0));
			front.vertices.push_back(lx::vec3(p, n, p));	front.uvs.push_back(lx::vec2(0.0, 1.0));
		}

		Face back;
		{
			back.vertices.push_back(lx::vec3(p, p, n));		back.uvs.push_back(lx::vec2(0.0, 0.0));
			back.vertices.push_back(lx::vec3(p, n, n));	back.uvs.push_back(lx::vec2(0.0, 1.0));
			back.vertices.push_back(lx::vec3(n, n, n));	back.uvs.push_back(lx::vec2(1.0, 1.0));
			back.vertices.push_back(lx::vec3(n, p, n));	back.uvs.push_back(lx::vec2(1.0, 0.0));
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




















