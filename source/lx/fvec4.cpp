// fvec4.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/fvec4.h"

#include <assert.h>

namespace lx
{
	float& fvec4::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	const float& fvec4::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	fvec4& fvec4::operator += (const fvec4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	fvec4& fvec4::operator -= (const fvec4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	fvec4& fvec4::operator *= (const fvec4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
	fvec4& fvec4::operator /= (const fvec4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }

	fvec4& fvec4::operator *= (float s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
	fvec4& fvec4::operator /= (float s) { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

	float fvec4::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w)); }

	fvec4 fvec4::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
		float ivs = _fastInverseSqrtF(mag);
		return fvec4(this->x * ivs, this->y * ivs, this->z * ivs, this->w * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		fvec4 fvec4::normalized() const { return this->normalised(); }
	#endif















	fvec4 operator + (const fvec4& a, const fvec4& b) { return fvec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	fvec4 operator - (const fvec4& a, const fvec4& b) { return fvec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	fvec4 operator * (const fvec4& a, const fvec4& b) { return fvec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	fvec4 operator / (const fvec4& a, const fvec4& b) { return fvec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	bool operator == (const fvec4& a, const fvec4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	fvec4 operator * (const fvec4& a, float b) { return fvec4(a.x * b, a.y * b, a.z * b, a.w * b); }
	fvec4 operator / (const fvec4& a, float b) { return fvec4(a.x / b, a.y / b, a.z / b, a.w / b); }

	fvec4 operator * (float a, const fvec4& b) { return b * a; }
	fvec4 operator / (float a, const fvec4& b) { return b / a; }

	fvec4 round(const fvec4&v) { return fvec4(lx::round(v.x), lx::round(v.y), lx::round(v.z), lx::round(v.w)); }
	fvec4 normalise(const fvec4& v) { return v.normalised(); }
	fvec4 normalize(const fvec4& v) { return v.normalised(); }
	float magnitude(const fvec4& v) { return v.magnitude(); }

	float dot(const fvec4& a, const fvec4& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	float distance(const fvec4& a, const fvec4& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
			+ ((a.w - b.w) * (a.w - b.w))
		);
	}
}














