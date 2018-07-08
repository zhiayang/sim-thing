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
			double ptr[0];
			vec3 vecs[3];
			struct {
				double i11; double i21; double i31;
				double i12; double i22; double i32;
				double i13; double i23; double i33;
			};
		};
		#pragma clang diagnostic pop

		// makes identity
		mat3x3();
		mat3x3(const vec3& a, const vec3& b, const vec3& c);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		mat3x3 (double a, double b, double c,
				double d, double e, double f,
				double g, double h, double i);


		vec3& operator[] (size_t i);
		const vec3& operator[] (size_t i) const;

		mat3x3& operator += (const mat3x3& m);
		mat3x3& operator -= (const mat3x3& m);

		mat3x3& operator *= (double s);
		mat3x3& operator /= (double s);

		mat4x4 translated(const vec3& v) const;
		mat3x3 rotated(double radians, const vec3& axis) const;
		mat3x3 scaled(const vec3& v) const;
		mat3x3 scaled(double s) const;
		mat3x3 transposed() const;

		double determinant() const;
		mat3x3 inversed() const;

		static mat3x3 identity();
		static mat3x3 zero();
	};

	// stuff
	using mat3 = mat3x3;



	vec3 operator * (const mat3x3& m, const vec3& v);
	mat3x3 operator * (const mat3x3& a, const mat3x3& b);

	mat3x3 operator * (const mat3x3& a, double s);
	mat3x3 operator * (double s, const mat3x3& a);

	mat3x3 operator / (const mat3x3& a, double s);
	mat3x3 operator / (double s, const mat3x3& a);

	bool operator == (const mat3x3& a, const mat3x3& b);

	mat3x3 rotate(const mat3x3& m, const vec3& axis, double rad);
	mat4x4 translate(const mat3x3& m, const vec3& v);
	mat3x3 scale(const mat3x3& m, const vec3& v);
	mat3x3 scale(const mat3x3& m, double d);
	mat3x3 transpose(const mat3x3& m);
	mat3x3 inverse(const mat3x3& m);
}







