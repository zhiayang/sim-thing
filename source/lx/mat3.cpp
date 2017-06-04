// mat3.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include <assert.h>

namespace lx
{
	mat3x3::mat3x3()
	{
		// make identity by default
		this->vecs[2] = vec3(0, 0, 1);
		this->vecs[1] = vec3(0, 1, 0);
		this->vecs[0] = vec3(1, 0, 0);
	}

	mat3x3::mat3x3(const vec3& a, const vec3& b, const vec3& c)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
		this->vecs[2] = c;
	}

	mat3x3 mat3x3::identity()
	{
		return mat3x3();
	}

	mat3x3 mat3x3::zero()
	{
		mat3x3 ret;
		ret.vecs[0] = vec3(0);
		ret.vecs[1] = vec3(0);
		ret.vecs[2] = vec3(0);

		return ret;
	}

	vec3& mat3x3::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->vecs[i];
	}

	const vec3& mat3x3::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->vecs[i];
	}

	mat3x3& mat3x3::operator += (const mat3x3& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		this->vecs[2] += m.vecs[2];
		return *this;
	}

	mat3x3& mat3x3::operator -= (const mat3x3& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		this->vecs[2] -= m.vecs[2];
		return *this;
	}

	mat3x3& mat3x3::operator *= (float s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		this->vecs[2] *= s;
		return *this;
	}

	mat3x3& mat3x3::operator /= (float s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		this->vecs[2] /= s;
		return *this;
	}


	mat4x4 mat3x3::translate(const vec3& v)
	{
		mat4x4 result;
		/*
			v1	v2	v3	v4
			1	0	0	x
			0	1	0	y
			0	0	1	z
			0	0	0	1
		*/
		auto x = v.x;
		auto y = v.y;
		auto z = v.z;

		result.vecs[3] = vec4(x, y, z, 1);
		result.vecs[2] = vec4(this->vecs[2], 0);
		result.vecs[1] = vec4(this->vecs[1], 0);
		result.vecs[0] = vec4(this->vecs[0], 0);

		return result;
	}

	mat3x3 mat3x3::rotate(float radians, const vec3& axis)
	{
		float c = lx::cos(radians);
		float s = lx::sin(radians);

		// bad form, i guess
		float C = (1 - c);
		float L = axis.x;
		float M = axis.y;
		float N = axis.z;

		mat3x3 ret;
		/*
			for axis = (L, M, N)

			v1				v2				v3
			LL(1-c) + c		ML(1-c) - Ns	NL(1-c) + Ms
			LM(1-c) + Ns	MM(1-c) + c		NM(1-c) - Ls
			LN(1-c) - Ms	MN(1-c) + Ls	NN(1-c) + c
		*/

		ret.vecs[2] = vec3(N * L * C + M * s,	N * M * C - L * s,	N * N * C + c);
		ret.vecs[1] = vec3(M * L * C - N * s,	M * M * C + c,		M * N * C + L * s);
		ret.vecs[0] = vec3(L * L * C + c,		L * M * C + N * s,	L * N * C - M * s);

		return ret * (*this);
	}

	mat3x3 mat3x3::scale(const vec3& v)
	{
		mat3x3 result;
		/*
			v1	v2	v3
			x	0	0
			0	y	0
			0	0	z
		*/
		// result.vecs[2] = vec3(0, 0, v.z);
		// result.vecs[1] = vec3(0, v.y, 0);
		// result.vecs[0] = vec3(v.x, 0, 0);

		result[0] = this->vecs[0] * v[0];
		result[1] = this->vecs[1] * v[1];
		result[2] = this->vecs[2] * v[2];

		return result;
	}

	mat3x3 mat3x3::scale(float s)
	{
		return this->scale(vec3(s));
	}










	vec3 operator * (const mat3x3& m, const vec3& v)
	{
		return (v.x * m[0]) + (v.y * m[1]) + (v.z * m[2]);
	}

	mat3x3 operator * (const mat3x3& a, const mat3x3& b)
	{
		mat3x3 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];
		ret.vecs[2] = a * b.vecs[2];

		return ret;
	}

	mat3x3 operator * (const mat3x3& a, float s) { return mat3x3(a.vecs[0] * s, a.vecs[1] * s, a.vecs[2] * s); }
	mat3x3 operator * (float s, const mat3x3& a) { return a * s; }

	mat3x3 operator / (const mat3x3& a, float s) { return mat3x3(a.vecs[0] / s, a.vecs[1] / s, a.vecs[2] / s); }
	mat3x3 operator / (float s, const mat3x3& a) { return a / s; }

	bool operator == (const mat3x3& a, const mat3x3& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1])
			&& (a.vecs[2] == b.vecs[2]);
	}
}




















