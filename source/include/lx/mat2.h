// mat2.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec2.h"
#include "lx/mat3.h"

namespace lx
{
	struct mat2x2
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			double ptr[0];
			vec2 vecs[2];
		};
		#pragma clang diagnostic pop

		// makes identity
		mat2x2();
		mat2x2(const vec2& a, const vec2& b);

		vec2& operator[] (size_t i);
		const vec2& operator[] (size_t i) const;

		mat2x2& operator += (const mat2x2& m);
		mat2x2& operator -= (const mat2x2& m);

		mat2x2& operator *= (double s);
		mat2x2& operator /= (double s);

		// rotate takes in values in radians, and operates anticlockwise (following the unit circle)
		mat3x3 translate(const vec2& v);
		mat2x2 rotate(double radians);
		mat2x2 scale(const vec2& v);
		mat2x2 scale(double s);

		static mat2x2 identity();
		static mat2x2 zero();
	};

	vec2 operator * (const mat2x2& m, const vec2& v);
	mat2x2 operator * (const mat2x2& a, const mat2x2& b);

	mat2x2 operator * (const mat2x2& a, double s);
	mat2x2 operator * (double s, const mat2x2& a);

	mat2x2 operator / (const mat2x2& a, double s);
	mat2x2 operator / (double s, const mat2x2& a);

	bool operator == (const mat2x2& a, const mat2x2& b);

	// stuff
	using mat2 = mat2x2;
}









