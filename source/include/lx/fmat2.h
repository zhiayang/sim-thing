// fmat2.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/fvec2.h"
#include "lx/fmat3.h"

namespace lx
{
	struct fmat2x2
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			fvec2 vecs[2];
			struct {
				float i11; float i21;
				float i12; float i22;
			};
		};
		#pragma clang diagnostic pop

		// makes identity
		fmat2x2();
		fmat2x2(const fvec2& a, const fvec2& b);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		fmat2x2(float a, float b,
				float c, float d);


		fvec2& operator[] (size_t i);
		const fvec2& operator[] (size_t i) const;

		fmat2x2& operator += (const fmat2x2& m);
		fmat2x2& operator -= (const fmat2x2& m);

		fmat2x2& operator *= (float s);
		fmat2x2& operator /= (float s);

		// rotate takes in values in radians, and operates anticlockwise (following the unit circle)
		fmat3x3 translated(const fvec2& v) const;
		fmat2x2 rotated(float radians) const;
		fmat2x2 scaled(const fvec2& v) const;
		fmat2x2 scaled(float s) const;
		fmat2x2 transposed() const;

		float determinant() const;
		fmat2x2 inversed() const;

		static fmat2x2 identity();
		static fmat2x2 zero();
	};

	fvec2 operator * (const fmat2x2& m, const fvec2& v);
	fmat2x2 operator * (const fmat2x2& a, const fmat2x2& b);

	fmat2x2 operator * (const fmat2x2& a, float s);
	fmat2x2 operator * (float s, const fmat2x2& a);

	fmat2x2 operator / (const fmat2x2& a, float s);
	fmat2x2 operator / (float s, const fmat2x2& a);

	bool operator == (const fmat2x2& a, const fmat2x2& b);

	fmat3x3 translate(const fmat2x2& m, const fvec2& v);
	fmat2x2 rotate(const fmat2x2& m, float rad);
	fmat2x2 scale(const fmat2x2& m, const fvec2& v);
	fmat2x2 scale(const fmat2x2& m, float d);
	fmat2x2 transpose(const fmat2x2& m);
	fmat2x2 inverse(const fmat2x2& m);

	// stuff
	using fmat2 = fmat2x2;
}









