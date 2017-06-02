// vec2.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/vec2.h"

#include <assert.h>


namespace lx
{
	// methods
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

	float& vec2::operator[] (size_t i)
	{
		assert(i == 0 || i == 1);
		return this->ptr[i];
	}

	const float& vec2::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1);
		return this->ptr[i];
	}

	vec2& vec2::operator += (const vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	vec2& vec2::operator -= (const vec2& v) { this->x -= v.x; this->y -= v.y; return *this; }
	vec2& vec2::operator *= (const vec2& v) { this->x *= v.x; this->y *= v.y; return *this; }
	vec2& vec2::operator /= (const vec2& v) { this->x /= v.x; this->y /= v.y; return *this; }

	vec2& vec2::operator *= (float s) { this->x *= s; this->y *= s; return *this; }
	vec2& vec2::operator /= (float s) { this->x /= s; this->y /= s; return *this; }


	float vec2::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y)); }

	vec2 vec2::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y);
		float ivs = _fastInverseSqrt(mag);
		return vec2(this->x * ivs, this->y * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		vec2 vec2::normalized() const { return this->normalised(); }
	#endif











	vec2 operator + (const vec2& a, const vec2& b) { return vec2(a.x + b.x, a.y + b.y); }
	vec2 operator - (const vec2& a, const vec2& b) { return vec2(a.x - b.x, a.y - b.y); }
	vec2 operator * (const vec2& a, const vec2& b) { return vec2(a.x * b.x, a.y * b.y); }
	vec2 operator / (const vec2& a, const vec2& b) { return vec2(a.x / b.x, a.y / b.y); }
	bool operator == (const vec2& a, const vec2& b) { return a.x == b.x && a.y == b.y; }

	vec2 operator * (const vec2& a, float b) { return vec2(a.x * b, a.y * b); }
	vec2 operator / (const vec2& a, float b) { return vec2(a.x / b, a.y / b); }

	vec2 operator * (float a, const vec2& b) { return b * a; }
	vec2 operator / (float a, const vec2& b) { return b / a; }

	vec2 round(const vec2&v) { return vec2(lx::round(v.x), lx::round(v.y)); }
	vec2 normalise(const vec2& v) { return v.normalised(); }
	vec2 normalize(const vec2& v) { return v.normalised(); }
	float magnitude(const vec2& v) { return v.magnitude(); }

	float dot(const vec2& a, const vec2& b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}

	float distance(const vec2& a, const vec2& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
		);
	}
}














