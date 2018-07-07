// fvec3.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/fvec3.h"

#include <assert.h>

namespace lx
{
	float& fvec3::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	const float& fvec3::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	fvec3& fvec3::operator += (const fvec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	fvec3& fvec3::operator -= (const fvec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	fvec3& fvec3::operator *= (const fvec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	fvec3& fvec3::operator /= (const fvec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }

	fvec3& fvec3::operator *= (float s) { this->x *= s; this->y *= s; this->z *= s; return *this; }
	fvec3& fvec3::operator /= (float s) { this->x /= s; this->y /= s; this->z /= s; return *this; }

	float fvec3::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }

	fvec3 fvec3::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
		float ivs = _fastInverseSqrtF(mag);
		return fvec3(this->x * ivs, this->y * ivs, this->z * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		fvec3 fvec3::normalized() const { return this->normalised(); }
	#endif



















	fvec3 operator + (const fvec3& a, const fvec3& b) { return fvec3(a.x + b.x, a.y + b.y, a.z + b.z); }
	fvec3 operator - (const fvec3& a, const fvec3& b) { return fvec3(a.x - b.x, a.y - b.y, a.z - b.z); }
	fvec3 operator * (const fvec3& a, const fvec3& b) { return fvec3(a.x * b.x, a.y * b.y, a.z * b.z); }
	fvec3 operator / (const fvec3& a, const fvec3& b) { return fvec3(a.x / b.x, a.y / b.y, a.z / b.z); }
	bool operator == (const fvec3& a, const fvec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

	fvec3 operator * (const fvec3& a, float b) { return fvec3(a.x * b, a.y * b, a.z * b); }
	fvec3 operator / (const fvec3& a, float b) { return fvec3(a.x / b, a.y / b, a.z / b); }

	fvec3 operator * (float a, const fvec3& b) { return b * a; }
	fvec3 operator / (float a, const fvec3& b) { return b / a; }

	fvec3 round(const fvec3&v) { return fvec3(lx::round(v.x), lx::round(v.y), lx::round(v.z)); }
	fvec3 normalise(const fvec3& v) { return v.normalised(); }
	fvec3 normalize(const fvec3& v) { return v.normalised(); }
	float magnitude(const fvec3& v) { return v.magnitude(); }

	float dot(const fvec3& a, const fvec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

	}

	fvec3 cross(const fvec3& a, const fvec3& b)
	{
		float x = (a.y * b.z) - (a.z * b.y);
		float y = (a.z * b.x) - (a.x * b.z);
		float z = (a.x * b.y) - (a.y * b.x);

		return fvec3(x, y, z);
	}

	float distance(const fvec3& a, const fvec3& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
		);
	}

}















