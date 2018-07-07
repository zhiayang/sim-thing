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
		PointLight(const lx::fvec3& p, const lx::fvec4& dc, const lx::fvec4& sc, float i, float r)
			: position(p), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r) { }

		lx::fvec3 position;

		lx::fvec4 diffuseColour;
		lx::fvec4 specularColour;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct SpotLight
	{
		SpotLight(const lx::fvec3& p, const lx::fvec3& dir, const lx::fvec4& dc, const lx::fvec4& sc, float i, float r, float inner, float outer)
			: position(p), direction(dir), diffuseColour(dc), specularColour(sc), intensity(i), lightRadius(r)
		{
			this->innerCutoffCosine = lx::cos(lx::toRadians(inner));
			this->outerCutoffCosine = lx::cos(lx::toRadians(outer));
		}

		lx::fvec3 position;
		lx::fvec3 direction;

		lx::fvec4 diffuseColour;
		lx::fvec4 specularColour;

		float innerCutoffCosine = 0;
		float outerCutoffCosine = 0;

		float intensity = 0;
		float lightRadius = 0;
	};

	struct DirectionalLight
	{
	};
}




















