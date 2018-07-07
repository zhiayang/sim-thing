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
		};
		#pragma clang diagnostic pop

		// makes identity
		fmat3x3();
		fmat3x3(const fvec3& a, const fvec3& b, const fvec3& c);

		fvec3& operator[] (size_t i);
		const fvec3& operator[] (size_t i) const;

		fmat3x3& operator += (const fmat3x3& m);
		fmat3x3& operator -= (const fmat3x3& m);

		fmat3x3& operator *= (float s);
		fmat3x3& operator /= (float s);

		fmat4x4 translate(const fvec3& v);
		fmat3x3 rotate(float radians, const fvec3& axis);
		fmat3x3 scale(const fvec3& v);
		fmat3x3 scale(float s);

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
}







