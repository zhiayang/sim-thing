// quat.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/quat.h"

#include <tuple>
#include <assert.h>

namespace lx
{
	double& quat::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}

	const double& quat::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->ptr[i];
	}


	mat3 quat::toRotationMatrix() const
	{
		return mat3();
	}

	quat quat::fromRotationMatrix(const mat3& r)
	{
		return quat();
	}

	quat quat::normalised() const
	{
		double msq = (this->w * this->w) + (this->x * this->x) + (this->y * this->y) + (this->z * this->z);

		if(abs(1.0 - msq) < 2.107342e-08)
			return *this * (2.0 / (1.0 + msq));
		else
			return *this * (1.0 / sqrt(msq));
	}

	double quat::magnitude() const
	{
		return sqrt((this->w * this->w) + (this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	}




	vec3 quat::toEulerRads() const
	{
		double sinr = +2.0 * (this->w * this->x + this->y * this->z);
		double cosr = +1.0 - 2.0 * (this->x * this->x + this->y * this->y);
		double roll = atan2(sinr, cosr);


		// use 90 degrees if out of range
		double sinp = +2.0 * (this->w * this->y - this->z * this->x);
		double pitch = abs(sinp) >= 1 ? copysign(PI / 2, sinp) : asin(sinp);


		double siny = +2.0 * (this->w * this->z + this->x * this->y);
		double cosy = +1.0 - 2.0 * (this->y * this->y + this->z * this->z);
		double yaw = atan2(siny, cosy);

		return vec3(roll, pitch, yaw);
	}

	vec3 quat::toEulerDegs() const
	{
		auto rads = this->toEulerRads();
		return vec3(toDegrees(rads.x), toDegrees(rads.y), toDegrees(rads.z));
	}

	quat quat::fromEulerDegs(const vec3& elr)
	{
		return quat::fromEulerRads(toRadians(elr));
	}

	quat quat::fromEulerRads(const vec3& elr)
	{
		// pitch
		auto s = vec3(sin(0.5 * elr.x), sin(0.5 * elr.y), sin(0.5 * elr.z));
		auto c = vec3(cos(0.5 * elr.x), cos(0.5 * elr.y), cos(0.5 * elr.z));

		return quat(c.x * c.y * c.z - s.x * s.y * s.z,
					s.x * c.y * c.z + c.x * s.y * s.z,
					c.x * s.y * c.z - s.x * c.y * s.z,
					c.x * c.y * s.z + s.x * s.y * c.z);
	}

	quat& quat::operator += (const quat& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	quat& quat::operator -= (const quat& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	quat& quat::operator *= (double s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
	quat& quat::operator /= (double s) { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

	quat operator + (const quat& a, const quat& b) { return quat(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	quat operator - (const quat& a, const quat& b) { return quat(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

	quat& quat::operator *= (const quat& v)
	{
		*this = *this * v;
		return *this;
	}

	quat operator * (const quat& a, const quat& b)
	{
		return quat(a.real * b.real - dot(a.imag, b.imag),
					vec3((a.real * b.imag) + (b.real * a.imag) + cross(a.imag, b.imag))
				).normalised();
	}


	bool operator == (const quat& a, const quat& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	quat operator * (const quat& a, double b) { return quat(a.x * b, a.y * b, a.z * b, a.w * b); }
	quat operator / (const quat& a, double b) { return quat(a.x / b, a.y / b, a.z / b, a.w / b); }
	quat operator * (double a, const quat& b) { return b * a; }


	double magnitude(const quat& q) { return q.magnitude(); }
	quat normalise(const quat& q) { return q.normalised(); }
}














