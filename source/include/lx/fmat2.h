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
		};
		#pragma clang diagnostic pop

		// makes identity
		fmat2x2();
		fmat2x2(const fvec2& a, const fvec2& b);

		fvec2& operator[] (size_t i);
		const fvec2& operator[] (size_t i) const;

		fmat2x2& operator += (const fmat2x2& m);
		fmat2x2& operator -= (const fmat2x2& m);

		fmat2x2& operator *= (float s);
		fmat2x2& operator /= (float s);

		// rotate takes in values in radians, and operates anticlockwise (following the unit circle)
		fmat3x3 translate(const fvec2& v);
		fmat2x2 rotate(float radians);
		fmat2x2 scale(const fvec2& v);
		fmat2x2 scale(float s);

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

	// stuff
	using fmat2 = fmat2x2;
}









