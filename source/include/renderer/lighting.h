// lighting.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glbinding/gl/types.h>

#include <vector>
#include <unordered_map>

namespace Rx
{
	struct PointLight
	{
		PointLight(glm::vec3 p, glm::vec4 dc, glm::vec4 sc, float i, float c, float l, float q)
		{
			this->position = p;
			this->intensity = i;

			this->diffuseColour = dc;
			this->specularColour = sc;

			this->constantFactor = c;
			this->linearFactor = l;
			this->quadFactor = q;
		}

		glm::vec3 position;
		float intensity = 0;

		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		float constantFactor = 0;
		float linearFactor = 0;
		float quadFactor = 0;
	};

	struct DirectionalLight
	{
	};

	struct SpotLight
	{
	};
}
