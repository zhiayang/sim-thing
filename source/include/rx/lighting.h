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

namespace rx
{
	struct PointLight
	{
		PointLight(glm::vec3 p, glm::vec4 dc, glm::vec4 sc, float i, float r)
		{
			this->position = p;
			this->intensity = i;

			this->diffuseColour = dc;
			this->specularColour = sc;

			this->lightRadius = r;
		}

		glm::vec3 position;
		float intensity = 0;

		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		float lightRadius = 0;
	};

	struct SpotLight
	{
	};

	struct DirectionalLight
	{
	};
}




















