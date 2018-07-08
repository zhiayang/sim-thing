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
			struct {
				double i11; double i21;
				double i12; double i22;
			};
		};
		#pragma clang diagnostic pop

		// makes identity
		mat2x2();
		mat2x2(const vec2& a, const vec2& b);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		mat2x2 (double a, double b,
				double c, double d);

		vec2& operator[] (size_t i);
		const vec2& operator[] (size_t i) const;

		mat2x2& operator += (const mat2x2& m);
		mat2x2& operator -= (const mat2x2& m);

		mat2x2& operator *= (double s);
		mat2x2& operator /= (double s);

		mat3x3 translated(const vec2& v) const;
		mat2x2 scaled(const vec2& v) const;
		mat2x2 scaled(double s) const;
		mat2x2 transposed() const;

		double determinant() const;
		mat2x2 inversed() const;

		// rotate takes in values in radians, and operates anticlockwise (following the unit circle)
		mat2x2 rotated(double radians) const;

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

	mat3x3 translate(const mat2x2& m, const vec2& v);
	mat2x2 rotate(const mat2x2& m, double rad);
	mat2x2 scale(const mat2x2& m, const vec2& v);
	mat2x2 scale(const mat2x2& m, double d);
	mat2x2 transpose(const mat2x2& m);
	mat2x2 inverse(const mat2x2& m);

	// stuff
	using mat2 = mat2x2;
}









