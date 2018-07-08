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
			struct {
				float i11; float i21; float i31; float i41;
				float i12; float i22; float i32; float i42;
				float i13; float i23; float i33; float i43;
				float i14; float i24; float i34; float i44;
			};
		};
		#pragma clang diagnostic pop

		fmat4x4();
		fmat4x4(const fvec4& a, const fvec4& b, const fvec4& c, const fvec4& d);

		/*
			allows specifying everything, row-by-row. aka:

			a b c
			d e f
			g h i

			can be passed as
			mat2x2(a, b, c, d, e, .. g, h, i)
		*/
		fmat4x4(float a, float b, float c, float d,
				float e, float f, float g, float h,
				float i, float j, float k, float l,
				float m, float n, float o, float p);


		fvec4& operator[] (size_t i);
		const fvec4& operator[] (size_t i) const;

		fmat4x4& operator += (const fmat4x4& m);
		fmat4x4& operator -= (const fmat4x4& m);

		fmat4x4& operator *= (float s);
		fmat4x4& operator /= (float s);

		fmat4x4 translated(const fvec3& v) const;
		fmat4x4 rotated(float radians, const fvec3& axis) const;
		fmat4x4 scaled(const fvec3& v) const;
		fmat4x4 scaled(float s) const;
		fmat4x4 transposed() const;

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


	fmat4x4 rotate(const fmat4x4& m, const fvec3& axis, float rad);
	fmat4x4 translate(const fmat4x4& m, const fvec3& v);
	fmat4x4 scale(const fmat4x4& m, const fvec3& v);
	fmat4x4 scale(const fmat4x4& m, float d);
	fmat4x4 transpose(const fmat4x4& m);

	// stuff
	using fmat4 = fmat4x4;
}










