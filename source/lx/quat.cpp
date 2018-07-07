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
		return quat::fromEulerRads(vec3(toRadians(elr.x), toRadians(elr.y), toRadians(elr.z)));
	}

	quat quat::fromEulerRads(const vec3& elr)
	{
		double cx = cos(elr.x * 0.5);
		double sx = sin(elr.x * 0.5);

		double cy = cos(elr.y * 0.5);
		double sy = sin(elr.y * 0.5);

		double cz = cos(elr.z * 0.5);
		double sz = sin(elr.z * 0.5);

		return quat(cz * cy * cx + sz * sy * sx,
					cz * sy * cx - sz * cy * sx,
					cz * cy * sx + sz * sy * cx,
					sz * cy * cx - cz * sy * sx);
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
					vec3((a.real * b.imag) +
						(b.real * a.imag) +
						cross(a.imag, b.imag))
					);
	}


	bool operator == (const quat& a, const quat& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	quat operator * (const quat& a, double b) { return quat(a.x * b, a.y * b, a.z * b, a.w * b); }
	quat operator / (const quat& a, double b) { return quat(a.x / b, a.y / b, a.z / b, a.w / b); }
	quat operator * (double a, const quat& b) { return b * a; }
}














