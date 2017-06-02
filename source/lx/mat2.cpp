// mat2.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include <assert.h>

namespace lx
{
	mat2x2::mat2x2()
	{
		// make identity by default
		this->vecs[1] = vec2(0, 1);
		this->vecs[0] = vec2(1, 0);
	}

	mat2x2::mat2x2(const vec2& a, const vec2& b)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
	}

	mat2x2 mat2x2::identity()
	{
		return mat2x2();
	}

	mat2x2 mat2x2::zero()
	{
		mat2x2 ret;
		ret.vecs[0] = vec2(0);
		ret.vecs[1] = vec2(0);

		return ret;
	}

	vec2& mat2x2::operator[] (size_t i)
	{
		assert(i == 0 || i == 1);
		return this->vecs[i];
	}

	const vec2& mat2x2::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1);
		return this->vecs[i];
	}

	mat2x2& mat2x2::operator += (const mat2x2& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		return *this;
	}

	mat2x2& mat2x2::operator -= (const mat2x2& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		return *this;
	}

	mat2x2& mat2x2::operator *= (float s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		return *this;
	}

	mat2x2& mat2x2::operator /= (float s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		return *this;
	}

	mat2x2 mat2x2::rotate(float angle)
	{
		float c = lx::cos(angle);
		float s = lx::sin(angle);
		float S = lx::sin(angle); // bad form, bla bla

		mat2x2 result;
		/*
			v1	v2
			c	-s
			s	c
		*/

		result.vecs[1] = vec2(S, c);
		result.vecs[0] = vec2(c, s);

		return result;
	}

	mat3x3 mat2x2::translate(const vec2& v)
	{
		mat3x3 result;
		/*
			v1	v2	v3
			1	0	x
			0	1	y
			0	0	1
		*/

		result.vecs[2] = vec3(v.x, v.y, 1);
		result.vecs[1] = vec3(0, 1, 0);
		result.vecs[0] = vec3(1, 0, 0);

		return result;
	}

	mat2x2 mat2x2::scale(const vec2& v)
	{
		mat2x2 result;
		/*
			v1	v2
			x	0
			0	y
		*/
		result.vecs[1] = vec2(0, v.y);
		result.vecs[0] = vec2(v.x, 0);

		return result;
	}

	mat2x2 mat2x2::scale(float s)
	{
		return this->scale(vec2(s));
	}








	vec2 operator * (const mat2x2& m, const vec2& v)
	{
		return (v.x * m[0]) + (v.y * m[1]);
	}

	mat2x2 operator * (const mat2x2& a, const mat2x2& b)
	{
		mat2x2 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];

		return ret;
	}

	mat2x2 operator * (const mat2x2& a, float s) { return mat2x2(a.vecs[0] * s, a.vecs[1] * s); }
	mat2x2 operator * (float s, const mat2x2& a) { return a * s; }

	mat2x2 operator / (const mat2x2& a, float s) { return mat2x2(a.vecs[0] / s, a.vecs[1] / s); }
	mat2x2 operator / (float s, const mat2x2& a) { return a / s; }

	bool operator == (const mat2x2& a, const mat2x2& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1]);
	}
}




















