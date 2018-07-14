// vec4.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/vec4.h"

#include <assert.h>

namespace lx
{
	double& vec4::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	const double& vec4::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	vec4 vec4::operator - () const { return vec4(-this->x, -this->y, -this->z, -this->w); }

	vec4& vec4::operator += (const vec4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	vec4& vec4::operator -= (const vec4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	vec4& vec4::operator *= (const vec4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
	vec4& vec4::operator /= (const vec4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }

	vec4& vec4::operator *= (double s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
	vec4& vec4::operator /= (double s) { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

	double vec4::magnitude() const { return sqrt(this->magnitudeSquared()); }
	double vec4::magnitudeSquared() const { return (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w); }

	vec4 vec4::normalised() const
	{
		double mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
		double ivs = _fastInverseSqrtD(mag);
		return vec4(this->x * ivs, this->y * ivs, this->z * ivs, this->w * ivs);
	}















	vec4 operator + (const vec4& a, const vec4& b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	vec4 operator - (const vec4& a, const vec4& b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	vec4 operator * (const vec4& a, const vec4& b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	vec4 operator / (const vec4& a, const vec4& b) { return vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	bool operator == (const vec4& a, const vec4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	vec4 operator * (const vec4& a, double b) { return vec4(a.x * b, a.y * b, a.z * b, a.w * b); }
	vec4 operator / (const vec4& a, double b) { return vec4(a.x / b, a.y / b, a.z / b, a.w / b); }

	vec4 operator * (double a, const vec4& b) { return b * a; }
	vec4 operator / (double a, const vec4& b) { return b / a; }

	vec4 round(const vec4&v) { return vec4(lx::round(v.x), lx::round(v.y), lx::round(v.z), lx::round(v.w)); }
	vec4 normalise(const vec4& v) { return v.normalised(); }
	double magnitude(const vec4& v) { return v.magnitude(); }
	double magnitudeSquared(const vec4& v) { return v.magnitudeSquared(); }

	double dot(const vec4& a, const vec4& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	double distance(const vec4& a, const vec4& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
			+ ((a.z - b.z) * (a.z - b.z))
			+ ((a.w - b.w) * (a.w - b.w))
		);
	}
}














