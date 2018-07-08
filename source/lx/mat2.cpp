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

	mat2x2::mat2x2(double a, double b, double c, double d)
	{
		this->vecs[0] = vec2(a, c);
		this->vecs[1] = vec2(b, d);
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

	mat2x2& mat2x2::operator *= (double s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		return *this;
	}

	mat2x2& mat2x2::operator /= (double s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		return *this;
	}

	mat2x2 mat2x2::rotated(double angle) const
	{
		double c = lx::cos(angle);
		double s = lx::sin(angle);
		double S = lx::sin(angle); // bad form, bla bla

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

	mat3x3 mat2x2::translated(const vec2& v) const
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

	mat2x2 mat2x2::scaled(const vec2& v) const
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

	mat2x2 mat2x2::scaled(double s) const
	{
		return this->scaled(vec2(s));
	}

	mat2x2 mat2x2::transposed() const
	{
		/*
			a b     a c
			c d     b d
		*/
		mat2x2 result;

		result.vecs[1] = vec2(this->vecs[0].y, this->vecs[1].y);
		result.vecs[0] = vec2(this->vecs[0].x, this->vecs[1].x);

		return result;
	}

	double mat2x2::determinant() const
	{
		return (i11 * i22) - (i21 * i12);
	}

	mat2x2 mat2x2::inversed() const
	{
		if(this->determinant() == 0)
		{
			assert(false && "cannot invert this matrix");
			return mat2x2();
		}

		return (1.0 / this->determinant()) * mat2x2(
			i22, -i12,
			-i21, i11
		);
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

	mat2x2 operator * (const mat2x2& a, double s) { return mat2x2(a.vecs[0] * s, a.vecs[1] * s); }
	mat2x2 operator * (double s, const mat2x2& a) { return a * s; }

	mat2x2 operator / (const mat2x2& a, double s) { return mat2x2(a.vecs[0] / s, a.vecs[1] / s); }
	mat2x2 operator / (double s, const mat2x2& a) { return a / s; }

	bool operator == (const mat2x2& a, const mat2x2& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1]);
	}

	mat3x3 translate(const mat2x2& m, const vec2& v)    { return m.translated(v); }
	mat2x2 rotate(const mat2x2& m, double rad)          { return m.rotated(rad); }
	mat2x2 transpose(const mat2x2& m)                   { return m.transposed(); }
	mat2x2 scale(const mat2x2& m, const vec2& v)        { return m.scaled(v); }
	mat2x2 scale(const mat2x2& m, double d)             { return m.scaled(d); }
}




















