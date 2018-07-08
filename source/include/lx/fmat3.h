// fmat3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/fvec3.h"
#include "lx/fmat4.h"

namespace lx
{
	struct fmat3x3
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			fvec3 vecs[3];
			struct {
				float i11; float i21; float i31;
				float i12; float i22; float i32;
				float i13; float i23; float i33;
			};
		};
		#pragma clang diagnostic pop

		// makes identity
		fmat3x3();
		fmat3x3(const fvec3& a, const fvec3& b, const fvec3& c);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		fmat3x3(float a, float b, float c,
				float d, float e, float f,
				float g, float h, float i);

		fvec3& operator[] (size_t i);
		const fvec3& operator[] (size_t i) const;

		fmat3x3& operator += (const fmat3x3& m);
		fmat3x3& operator -= (const fmat3x3& m);

		fmat3x3& operator *= (float s);
		fmat3x3& operator /= (float s);

		fmat4x4 translated(const fvec3& v) const;
		fmat3x3 rotated(float radians, const fvec3& axis) const;
		fmat3x3 scaled(const fvec3& v) const;
		fmat3x3 scaled(float s) const;
		fmat3x3 transposed() const;

		float determinant() const;
		fmat3x3 inversed() const;

		static fmat3x3 identity();
		static fmat3x3 zero();
	};

	// stuff
	using fmat3 = fmat3x3;



	fvec3 operator * (const fmat3x3& m, const fvec3& v);
	fmat3x3 operator * (const fmat3x3& a, const fmat3x3& b);

	fmat3x3 operator * (const fmat3x3& a, float s);
	fmat3x3 operator * (float s, const fmat3x3& a);

	fmat3x3 operator / (const fmat3x3& a, float s);
	fmat3x3 operator / (float s, const fmat3x3& a);

	bool operator == (const fmat3x3& a, const fmat3x3& b);

	fmat3x3 rotate(const fmat3x3& m, const fvec3& axis, float rad);
	fmat4x4 translate(const fmat3x3& m, const fvec3& v);
	fmat3x3 scale(const fmat3x3& m, const fvec3& v);
	fmat3x3 scale(const fmat3x3& m, float d);
	fmat3x3 transpose(const fmat3x3& m);
	fmat3x3 inverse(const fmat3x3& m);
}







