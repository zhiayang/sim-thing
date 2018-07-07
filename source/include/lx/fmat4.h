// fmat3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/fvec4.h"

namespace lx
{
	struct fmat4x4
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			fvec4 vecs[4];
		};
		#pragma clang diagnostic pop

		fmat4x4();
		fmat4x4(const fvec4& a, const fvec4& b, const fvec4& c, const fvec4& d);

		fvec4& operator[] (size_t i);
		const fvec4& operator[] (size_t i) const;

		fmat4x4& operator += (const fmat4x4& m);
		fmat4x4& operator -= (const fmat4x4& m);

		fmat4x4& operator *= (float s);
		fmat4x4& operator /= (float s);

		fmat4x4 translate(const fvec3& v);
		fmat4x4 rotate(float radians, const fvec3& axis);
		fmat4x4 scale(const fvec3& v);
		fmat4x4 scale(float s);

		static fmat4x4 identity();
		static fmat4x4 zero();
	};


	fvec4 operator * (const fmat4x4& m, const fvec4& v);
	fmat4x4 operator * (const fmat4x4& a, const fmat4x4& b);

	fmat4x4 operator * (const fmat4x4& a, float s);
	fmat4x4 operator * (float s, const fmat4x4& a);

	fmat4x4 operator / (const fmat4x4& a, float s);
	fmat4x4 operator / (float s, const fmat4x4& a);

	bool operator == (const fmat4x4& a, const fmat4x4& b);


	// stuff
	using fmat4 = fmat4x4;
}










