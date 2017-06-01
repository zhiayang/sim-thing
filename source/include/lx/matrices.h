// matrices.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vectors.h"

namespace lx
{
	struct mat2x2
	{
		union
		{
			float ptr[0];
			vec2 vecs[2];
		};

		vec2& operator[] (size_t i) { return this->vecs[i]; }
		const vec2& operator[] (size_t i) const { return this->vecs[i]; }
	};

	struct mat3x3
	{
		union
		{
			float ptr[0];
			vec3 vecs[3];
		};

		vec3& operator[] (size_t i) { return this->vecs[i]; }
		const vec3& operator[] (size_t i) const { return this->vecs[i]; }
	};

	struct mat4x4
	{
		union
		{
			float ptr[0];
			vec4 vecs[2];
		};

		vec4& operator[] (size_t i) { return this->vecs[i]; }
		const vec4& operator[] (size_t i) const { return this->vecs[i]; }
	};


	// stuff
	using mat2 = mat2x2;
	using mat3 = mat3x3;
	using mat4 = mat4x4;
}



