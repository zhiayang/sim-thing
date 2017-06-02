// mat3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec3.h"
#include "lx/mat4.h"

namespace lx
{
	struct mat3x3
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			vec3 vecs[3];
		};
		#pragma clang diagnostic pop

		// makes identity
		mat3x3();
		mat3x3(const vec3& a, const vec3& b, const vec3& c);

		vec3& operator[] (size_t i);
		const vec3& operator[] (size_t i) const;

		mat3x3& operator += (const mat3x3& m);
		mat3x3& operator -= (const mat3x3& m);

		mat3x3& operator *= (float s);
		mat3x3& operator /= (float s);

		mat4x4 translate(const vec3& v);
		mat3x3 rotate(float radians, const vec3& axis);
		mat3x3 scale(const vec3& v);
		mat3x3 scale(float s);

		static mat3x3 identity();
		static mat3x3 zero();
	};

	// stuff
	using mat3 = mat3x3;



	vec3 operator * (const mat3x3& m, const vec3& v);
	mat3x3 operator * (const mat3x3& a, const mat3x3& b);

	mat3x3 operator * (const mat3x3& a, float s);
	mat3x3 operator * (float s, const mat3x3& a);

	mat3x3 operator / (const mat3x3& a, float s);
	mat3x3 operator / (float s, const mat3x3& a);

	bool operator == (const mat3x3& a, const mat3x3& b);
}







