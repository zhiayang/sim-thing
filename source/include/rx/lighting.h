// lighting.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

#include <glbinding/gl/types.h>

#include <vector>
#include <unordered_map>

#include "lx.h"

namespace rx
{
	struct PointLight
	{
		PointLight(lx::vec3 p, lx::vec4 dc, lx::vec4 sc, float i, float r)
			: position(p), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r) { }

		lx::vec3 position;

		lx::vec4 diffuseColour;
		lx::vec4 specularColour;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct SpotLight
	{
		SpotLight(lx::vec3 p, lx::vec3 dir, lx::vec4 dc, lx::vec4 sc, float i, float r, float inner, float outer)
			: position(p), direction(dir), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r)
		{
			this->innerCutoffCosine = lx::cos(lx::toRadians(inner));
			this->outerCutoffCosine = lx::cos(lx::toRadians(outer));
		}

		lx::vec3 position;
		lx::vec3 direction;

		lx::vec4 diffuseColour;
		lx::vec4 specularColour;

		float innerCutoffCosine = 0;
		float outerCutoffCosine = 0;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct DirectionalLight
	{
	};
}




















