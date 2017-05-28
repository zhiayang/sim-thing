// model.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "assetloader.h"

#include "renderer/misc.h"

namespace Rx
{
	struct Face
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		glm::vec3 faceNormal;
	};

	struct Material
	{
		Material() { }
		Material(glm::vec4 amb, glm::vec4 dif, glm::vec4 spc, float s)
			: hasValue(true), ambientColour(amb), diffuseColour(dif), specularColour(spc), shine(s) { }

		Material(glm::vec4 amb, Texture* diffuse, Texture* specular, float s)
			: hasValue(true), ambientColour(amb), diffuseMap(diffuse), specularMap(specular), shine(s) { }

		bool hasValue = false;

		glm::vec4 ambientColour;
		float shine = 0;

		// possible to use colours
		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		// or textures
		Texture* diffuseMap = 0;
		Texture* specularMap = 0;
	};

	struct Mesh
	{
		Mesh();

		std::string name;
		id_t id;

		std::vector<Face> faces;

		static Mesh getUnitCube();
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
	glm::vec3 calculateNormalForFace(Face face);

	Model loadModelFromAsset(AssetLoader::Asset* asset, double scale);


	// obj files
	Model loadModelFromWavefrontOBJ(AssetLoader::Asset* asset, double scale);
}























