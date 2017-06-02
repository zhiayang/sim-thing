// lighting.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/trigonometric.hpp>

#include <glbinding/gl/types.h>

#include <vector>
#include <unordered_map>

#include <math.h>

namespace rx
{
	struct PointLight
	{
		PointLight(glm::vec3 p, glm::vec4 dc, glm::vec4 sc, float i, float r)
			: position(p), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r) { }

		glm::vec3 position;

		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct SpotLight
	{
		SpotLight(glm::vec3 p, glm::vec3 dir, glm::vec4 dc, glm::vec4 sc, float i, float r, float inner, float outer)
			: position(p), direction(dir), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r)
		{
			this->innerCutoffCosine = glm::cos(glm::radians(inner));
			this->outerCutoffCosine = glm::cos(glm::radians(outer));
		}

		glm::vec3 position;
		glm::vec3 direction;

		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		float innerCutoffCosine = 0;
		float outerCutoffCosine = 0;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct DirectionalLight
	{
	};
}




















