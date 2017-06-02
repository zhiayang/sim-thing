// mat3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec4.h"

namespace lx
{
	struct mat4x4
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			vec4 vecs[4];
		};
		#pragma clang diagnostic pop

		mat4x4();
		mat4x4(const vec4& a, const vec4& b, const vec4& c, const vec4& d);

		vec4& operator[] (size_t i);
		const vec4& operator[] (size_t i) const;

		mat4x4& operator += (const mat4x4& m);
		mat4x4& operator -= (const mat4x4& m);

		mat4x4& operator *= (float s);
		mat4x4& operator /= (float s);

		mat4x4 translate(const vec3& v);
		mat4x4 rotate(float radians, const vec3& axis);
		mat4x4 scale(const vec3& v);
		mat4x4 scale(float s);

		static mat4x4 identity();
		static mat4x4 zero();
	};


	vec4 operator * (const mat4x4& m, const vec4& v);
	mat4x4 operator * (const mat4x4& a, const mat4x4& b);

	mat4x4 operator * (const mat4x4& a, float s);
	mat4x4 operator * (float s, const mat4x4& a);

	mat4x4 operator / (const mat4x4& a, float s);
	mat4x4 operator / (float s, const mat4x4& a);

	bool operator == (const mat4x4& a, const mat4x4& b);


	// stuff
	using mat4 = mat4x4;
}










