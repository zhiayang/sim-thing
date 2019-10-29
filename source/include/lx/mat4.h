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
			double ptr[0];
			vec4 vecs[4];
			struct {
				double i11; double i21; double i31; double i41;
				double i12; double i22; double i32; double i42;
				double i13; double i23; double i33; double i43;
				double i14; double i24; double i34; double i44;
			};
		};
		#pragma clang diagnostic pop

		mat4x4();
		mat4x4(const vec4& a, const vec4& b, const vec4& c, const vec4& d);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		mat4x4 (double a, double b, double c, double d,
				double e, double f, double g, double h,
				double i, double j, double k, double l,
				double m, double n, double o, double p);

		vec4& operator[] (size_t i);
		const vec4& operator[] (size_t i) const;

		mat4x4& operator += (const mat4x4& m);
		mat4x4& operator -= (const mat4x4& m);

		mat4x4& operator *= (double s);
		mat4x4& operator /= (double s);

		mat4x4 translated(const vec3& v) const;
		mat4x4 rotated(double radians, const vec3& axis) const;
		mat4x4 scaled(const vec3& v) const;
		mat4x4 scaled(double s) const;
		mat4x4 transposed() const;

		mat4x4 rotationOnly() const;

		static mat4x4 identity();
		static mat4x4 zero();
	};


	vec4 operator * (const mat4x4& m, const vec4& v);
	mat4x4 operator * (const mat4x4& a, const mat4x4& b);

	mat4x4 operator * (const mat4x4& a, double s);
	mat4x4 operator * (double s, const mat4x4& a);

	mat4x4 operator / (const mat4x4& a, double s);
	mat4x4 operator / (double s, const mat4x4& a);

	bool operator == (const mat4x4& a, const mat4x4& b);


	mat4x4 rotate(const mat4x4& m, const vec3& axis, double rad);
	mat4x4 translate(const mat4x4& m, const vec3& v);
	mat4x4 scale(const mat4x4& m, const vec3& v);
	mat4x4 scale(const mat4x4& m, double d);
	mat4x4 transpose(const mat4x4& m);

	// stuff
	using mat4 = mat4x4;
}










