// vec4.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/vec4.h"

#include <assert.h>

namespace lx
{
	static float _fastInverseSqrt(float number)
	{
		int32_t i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = *(int32_t*) &y;					// evil floating point bit level hacking
		i  = 0x5F3759DF - (i >> 1);				// what the fuck?
		y  = *(float*) &i;
		y  = y * (threehalfs - (x2 * y * y));	// 1st iteration
		y  = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed

		return y;
	}

	float& vec4::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	const float& vec4::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	vec4& vec4::operator += (const vec4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	vec4& vec4::operator -= (const vec4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	vec4& vec4::operator *= (const vec4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
	vec4& vec4::operator /= (const vec4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }

	vec4& vec4::operator *= (float s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
	vec4& vec4::operator /= (float s) { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

	float vec4::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w)); }

	vec4 vec4::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
		float ivs = _fastInverseSqrt(mag);
		return vec4(this->x * ivs, this->y * ivs, this->z * ivs, this->w * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		vec4 vec4::normalized() const { return this->normalised(); }
	#endif















	vec4 operator + (const vec4& a, const vec4& b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	vec4 operator - (const vec4& a, const vec4& b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	vec4 operator * (const vec4& a, const vec4& b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	vec4 operator / (const vec4& a, const vec4& b) { return vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	bool operator == (const vec4& a, const vec4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	vec4 operator * (const vec4& a, float b) { return vec4(a.x * b, a.y * b, a.z * b, a.w * b); }
	vec4 operator / (const vec4& a, float b) { return vec4(a.x / b, a.y / b, a.z / b, a.w / b); }

	vec4 operator * (float a, const vec4& b) { return b * a; }
	vec4 operator / (float a, const vec4& b) { return b / a; }

	vec4 round(const vec4&v) { return vec4(lx::round(v.x), lx::round(v.y), lx::round(v.z), lx::round(v.w)); }
	vec4 normalise(const vec4& v) { return v.normalised(); }
	vec4 normalize(const vec4& v) { return v.normalised(); }
	float magnitude(const vec4& v) { return v.magnitude(); }

	float dot(const vec4& a, const vec4& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	float distance(const vec4& a, const vec4& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
			+ ((a.w - b.w) * (a.w - b.w))
		);
	}
}














