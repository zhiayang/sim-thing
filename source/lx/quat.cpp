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


	double quat::angle() const
	{
		return 2 * acos(w);
	}

	vec3 quat::axis() const
	{
		double sq = _fastInverseSqrtD(1 - (w * w));

		return vec3(
			x * sq,
			y * sq,
			z * sq
		);
	}

	mat3 quat::toRotationMatrix() const
	{
		return mat3(
			vec3(1 - (2 * y * y) - (2 * z * z), (2 * x * y) + (2 * w * z), (2 * x * z) - (2 * w * y)),
			vec3((2 * x * y) - (2 * w * z), 1 - (2 * x * x) - (2 * z * z), (2 * y * z) - (2 * w * x)),
			vec3((2 * x * z) + (2 * w * y), (2 * y * z) - (2 * w * x), 1 - (2 * x * x) - (2 * y * y))
		);
	}

	quat quat::fromRotationMatrix(const mat3& m)
	{
		// copied from glm/include/gtc/quaternion.inl

		double fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		double fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		double fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		double fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		double fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		double biggestVal = sqrt(fourBiggestSquaredMinus1 + 1) * 0.5;
		double mult = 0.25 / biggestVal;

		switch(biggestIndex)
		{
			case 0:
				return quat(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);

			case 1:
				return quat((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);

			case 2:
				return quat((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);

			case 3:
				return quat((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);

			default:
				assert(false);
				return quat();
		}
	}

	quat quat::normalised() const
	{
		double msq = (this->w * this->w) + (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
		if(msq == 0)
			return quat();

		// if(abs(1.0 - msq) < 2.107342e-08)
		// 	return *this * (2.0 / (1.0 + msq));
		// else
		// 	return *this * (1.0 / sqrt(msq));

		auto oneoverlen = 1.0 / this->magnitude();
		return quat(w * oneoverlen, x * oneoverlen, y * oneoverlen, z * oneoverlen);
	}

	quat quat::conjugated() const
	{
		return quat(w, -x, -y, -z);
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

	quat quat::inversed() const
	{
		return this->conjugated() / dot(*this, *this);
	}

	quat& quat::operator += (const quat& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	quat& quat::operator -= (const quat& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	quat& quat::operator *= (double s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
	quat& quat::operator /= (double s) { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

	quat operator + (const quat& a, const quat& b) { return quat(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z); }
	quat operator - (const quat& a, const quat& b) { return quat(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z); }

	quat& quat::operator *= (const quat& v)
	{
		*this = *this * v;
		return *this;
	}

	quat operator * (const quat& a, const quat& b)
	{
		return quat(a.real * b.real - dot(a.imag, b.imag),
					vec3((a.real * b.imag) + (b.real * a.imag) + cross(a.imag, b.imag))
				);

		// return quat(
		// 	(a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
		// 	(a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
		// 	(a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
		// 	(a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w)
		// );
	}


	bool operator == (const quat& a, const quat& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	quat operator * (const quat& a, double b) { return quat(a.w * b, a.x * b, a.y * b, a.z * b); }
	quat operator / (const quat& a, double b) { return quat(a.w / b, a.x / b, a.y / b, a.z / b); }
	quat operator * (double a, const quat& b) { return b * a; }



	vec3 operator * (const quat& q, const vec3& v)
	{
		return ((q * quat(0, v)) * q.inversed()).imag;
	}

	vec3 operator * (const vec3& v, const quat& q)
	{
		return q * v;
	}

	fvec3 operator * (const quat& q, const fvec3& v)
	{
		return tof(q * fromf(v));
	}

	fvec3 operator * (const fvec3& v, const quat& q)
	{
		return q * v;
	}



	double dot(const quat& a, const quat& b)
	{
		return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	}

	double magnitude(const quat& q) { return q.magnitude(); }
	quat normalise(const quat& q) { return q.normalised(); }
	quat conjugate(const quat& q) { return q.conjugated(); }
	quat inverse(const quat& q) { return q.inversed(); }
}














