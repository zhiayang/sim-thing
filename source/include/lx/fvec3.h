// fvec3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/fvec2.h"

namespace lx
{
	struct fvec3
	{
		fvec3(float a, float b, float c) : x(a), y(b), z(c) { }

		fvec3() : x(0.0f), y(0.0f), z(0.0f) { }
		explicit fvec3(float a) : x(a), y(a), z(a) { }
		explicit fvec3(const fvec2& v2, float c) : x(v2.x), y(v2.y), z(c) { }


		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			struct { float x; float y; float z; };
			struct { float r; float g; float b; };
			struct { float s; float t; float u; };
		};
		#pragma clang diagnostic pop

		float& operator[] (size_t i);
		const float& operator[] (size_t i) const;
		fvec3 operator - () const;

		fvec3& operator += (const fvec3& v);
		fvec3& operator -= (const fvec3& v);
		fvec3& operator *= (const fvec3& v);
		fvec3& operator /= (const fvec3& v);

		fvec3& operator *= (float s);
		fvec3& operator /= (float s);

		float magnitudeSquared() const;
		float magnitude() const;
		fvec3 normalised() const;


		// swizzling
		fvec2 xy() const;
		fvec2 yx() const;

		fvec3 xyz() const;
		fvec3 xzy() const;
		fvec3 yxz() const;
		fvec3 yzx() const;
		fvec3 zxy() const;
		fvec3 zyx() const;

		fvec3 rgb() const;
		fvec3 bgr() const;

		fvec2 xx() const;
		fvec2 yy() const;
		fvec2 zz() const;
		fvec3 xxx() const;
		fvec3 yyy() const;
		fvec3 zzz() const;
	};

	fvec3 operator + (const fvec3& a, const fvec3& b);
	fvec3 operator - (const fvec3& a, const fvec3& b);
	fvec3 operator * (const fvec3& a, const fvec3& b);
	fvec3 operator / (const fvec3& a, const fvec3& b);
	bool operator == (const fvec3& a, const fvec3& b);

	fvec3 operator * (const fvec3& a, float b);
	fvec3 operator / (const fvec3& a, float b);
	fvec3 operator * (float a, const fvec3& b);
	fvec3 operator / (float a, const fvec3& b);

	fvec3 round(const fvec3& v);
	fvec3 normalise(const fvec3& v);
	float magnitude(const fvec3& v);
	float magnitudeSquared(const fvec3& v);
	fvec3 cross(const fvec3& a, const fvec3& b);

	float dot(const fvec3& a, const fvec3& b);
	float distance(const fvec3& a, const fvec3& b);
}
















