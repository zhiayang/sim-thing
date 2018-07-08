// vec3.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/vec3.h"

#include <assert.h>

namespace lx
{
	double& vec3::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	const double& vec3::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->ptr[i];
	}

	vec3 vec3::operator - () const { return vec3(-this->x, -this->y, -this->z); }

	vec3& vec3::operator += (const vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	vec3& vec3::operator -= (const vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	vec3& vec3::operator *= (const vec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	vec3& vec3::operator /= (const vec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }

	vec3& vec3::operator *= (double s) { this->x *= s; this->y *= s; this->z *= s; return *this; }
	vec3& vec3::operator /= (double s) { this->x /= s; this->y /= s; this->z /= s; return *this; }

	double vec3::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }

	vec3 vec3::normalised() const
	{
		double mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
		double ivs = _fastInverseSqrtD(mag);
		return vec3(this->x * ivs, this->y * ivs, this->z * ivs);
	}

















	vec3 operator + (const vec3& a, const vec3& b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
	vec3 operator - (const vec3& a, const vec3& b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
	vec3 operator * (const vec3& a, const vec3& b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
	vec3 operator / (const vec3& a, const vec3& b) { return vec3(a.x / b.x, a.y / b.y, a.z / b.z); }
	bool operator == (const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

	vec3 operator * (const vec3& a, double b) { return vec3(a.x * b, a.y * b, a.z * b); }
	vec3 operator / (const vec3& a, double b) { return vec3(a.x / b, a.y / b, a.z / b); }

	vec3 operator * (double a, const vec3& b) { return b * a; }
	vec3 operator / (double a, const vec3& b) { return b / a; }

	vec3 round(const vec3&v) { return vec3(lx::round(v.x), lx::round(v.y), lx::round(v.z)); }
	vec3 normalise(const vec3& v) { return v.normalised(); }
	double magnitude(const vec3& v) { return v.magnitude(); }

	double dot(const vec3& a, const vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

	}

	vec3 cross(const vec3& a, const vec3& b)
	{
		double x = (a.y * b.z) - (a.z * b.y);
		double y = (a.z * b.x) - (a.x * b.z);
		double z = (a.x * b.y) - (a.y * b.x);

		return vec3(x, y, z);
	}

	double distance(const vec3& a, const vec3& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
		);
	}

}















