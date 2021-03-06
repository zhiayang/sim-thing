// model.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

#include "assetloader.h"

#include "rx/misc.h"

namespace rx
{
	struct RenderObject;


	struct Face
	{
		std::vector<lx::vec3> vertices;
		std::vector<lx::vec3> normals;
		std::vector<lx::vec2> uvs;

		lx::vec3 faceNormal;
	};

	struct Material
	{
		Material() : hasValue(false) { }
		Material(lx::vec4 amb, lx::vec4 dif, lx::vec4 spc, float s)
			: hasValue(true), ambientColour(amb), diffuseColour(dif), specularColour(spc), shine(s) { }

		Material(lx::vec4 amb, Texture* diffuse, Texture* specular, float s)
			: hasValue(true), ambientColour(amb), diffuseColour(util::colour::white()), specularColour(util::colour::white()),
				diffuseMap(diffuse), specularMap(specular), shine(s) { }

		bool hasValue = false;

		lx::vec4 ambientColour;

		// possible to use colours
		lx::vec4 diffuseColour;
		lx::vec4 specularColour;

		// or textures
		Texture* diffuseMap = 0;
		Texture* specularMap = 0;

		float shine = 0;
	};

	struct Mesh
	{
		Mesh();

		std::string name;
		id_t id;

		std::vector<Face> faces;

		static Mesh getUnitCube(const lx::vec3& scale = lx::vec3(1));
		static Mesh getUnitCube(double scale);
	};

	struct Model
	{
		Model();

		void addMesh(const Mesh& mesh, const Material& mat);

		std::string name;
		id_t id;

		std::vector<std::pair<Mesh, Material>> objects;

		static Model fromMesh(Mesh mesh, Material mat);
	};


	// given 4 vertices, returns 6 vertices which are the triangles of the quad.
	Face triangulateQuadFace(Face face);
	lx::vec3 calculateNormalForFace(Face face);

	Model loadModelFromAsset(AssetLoader::Asset* asset, double scale);


	// obj files
	Model loadModelFromWavefrontOBJ(AssetLoader::Asset* asset, double scale);
}























