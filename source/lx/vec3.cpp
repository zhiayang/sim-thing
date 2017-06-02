// vec3.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/vec3.h"

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

	float& vec3::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	const float& vec3::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	vec3& vec3::operator += (const vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	vec3& vec3::operator -= (const vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	vec3& vec3::operator *= (const vec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	vec3& vec3::operator /= (const vec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }

	vec3& vec3::operator *= (float s) { this->x *= s; this->y *= s; this->z *= s; return *this; }
	vec3& vec3::operator /= (float s) { this->x /= s; this->y /= s; this->z /= s; return *this; }

	float vec3::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }

	vec3 vec3::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
		float ivs = _fastInverseSqrt(mag);
		return vec3(this->x * ivs, this->y * ivs, this->z * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		vec3 vec3::normalized() const { return this->normalised(); }
	#endif



















	vec3 operator + (const vec3& a, const vec3& b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
	vec3 operator - (const vec3& a, const vec3& b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
	vec3 operator * (const vec3& a, const vec3& b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
	vec3 operator / (const vec3& a, const vec3& b) { return vec3(a.x / b.x, a.y / b.y, a.z / b.z); }
	bool operator == (const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

	vec3 operator * (const vec3& a, float b) { return vec3(a.x * b, a.y * b, a.z * b); }
	vec3 operator / (const vec3& a, float b) { return vec3(a.x / b, a.y / b, a.z / b); }

	vec3 operator * (float a, const vec3& b) { return b * a; }
	vec3 operator / (float a, const vec3& b) { return b / a; }

	vec3 round(const vec3&v) { return vec3(lx::round(v.x), lx::round(v.y), lx::round(v.z)); }
	vec3 normalise(const vec3& v) { return v.normalised(); }
	vec3 normalize(const vec3& v) { return v.normalised(); }
	float magnitude(const vec3& v) { return v.magnitude(); }

	float dot(const vec3& a, const vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

	}

	vec3 cross(const vec3& a, const vec3& b)
	{
		float x = (a.y * b.z) - (a.z * b.y);
		float y = (a.z * b.x) - (a.x * b.z);
		float z = (a.x * b.y) - (a.y * b.x);

		return vec3(x, y, z);
	}

	float distance(const vec3& a, const vec3& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
		);
	}

}















