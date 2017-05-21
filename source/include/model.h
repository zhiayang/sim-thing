// model.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

#include <glm/fwd.hpp>

#include "assetloader.h"

namespace Rx
{
	struct Model
	{
		std::string name;
		id_t id;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
	};

	Model* loadModelFromAsset(AssetLoader::Asset* asset);
}






