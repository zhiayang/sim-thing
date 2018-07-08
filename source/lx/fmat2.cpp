// fmat2.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include <assert.h>

namespace lx
{
	fmat2x2::fmat2x2()
	{
		// make identity by default
		this->vecs[1] = fvec2(0, 1);
		this->vecs[0] = fvec2(1, 0);
	}

	fmat2x2::fmat2x2(const fvec2& a, const fvec2& b)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
	}

	fmat2x2::fmat2x2(float a, float b, float c, float d)
	{
		this->vecs[0] = fvec2(a, c);
		this->vecs[1] = fvec2(b, d);
	}

	fmat2x2 fmat2x2::identity()
	{
		return fmat2x2();
	}

	fmat2x2 fmat2x2::zero()
	{
		fmat2x2 ret;
		ret.vecs[0] = fvec2(0);
		ret.vecs[1] = fvec2(0);

		return ret;
	}

	fvec2& fmat2x2::operator[] (size_t i)
	{
		assert(i == 0 || i == 1);
		return this->vecs[i];
	}

	const fvec2& fmat2x2::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1);
		return this->vecs[i];
	}

	fmat2x2& fmat2x2::operator += (const fmat2x2& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		return *this;
	}

	fmat2x2& fmat2x2::operator -= (const fmat2x2& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		return *this;
	}

	fmat2x2& fmat2x2::operator *= (float s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		return *this;
	}

	fmat2x2& fmat2x2::operator /= (float s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		return *this;
	}

	fmat2x2 fmat2x2::rotated(float angle) const
	{
		float c = lx::cos(angle);
		float s = lx::sin(angle);
		float S = lx::sin(angle); // bad form, bla bla

		fmat2x2 result;
		/*
			v1	v2
			c	-s
			s	c
		*/

		result.vecs[1] = fvec2(S, c);
		result.vecs[0] = fvec2(c, s);

		return result;
	}

	fmat3x3 fmat2x2::translated(const fvec2& v) const
	{
		fmat3x3 result;
		/*
			v1	v2	v3
			1	0	x
			0	1	y
			0	0	1
		*/

		result.vecs[2] = fvec3(v.x, v.y, 1);
		result.vecs[1] = fvec3(0, 1, 0);
		result.vecs[0] = fvec3(1, 0, 0);

		return result;
	}

	fmat2x2 fmat2x2::scaled(const fvec2& v) const
	{
		fmat2x2 result;
		/*
			v1	v2
			x	0
			0	y
		*/
		result.vecs[1] = fvec2(0, v.y);
		result.vecs[0] = fvec2(v.x, 0);

		return result;
	}

	fmat2x2 fmat2x2::scaled(float s) const
	{
		return this->scaled(fvec2(s));
	}


	fmat2x2 fmat2x2::transposed() const
	{
		/*
			a b     a c
			c d     b d
		*/
		fmat2x2 result;

		result.vecs[1] = fvec2(this->vecs[0].y, this->vecs[1].y);
		result.vecs[0] = fvec2(this->vecs[0].x, this->vecs[1].x);

		return result;
	}

	float fmat2x2::determinant() const
	{
		return (i11 * i22) - (i21 * i12);
	}

	fmat2x2 fmat2x2::inversed() const
	{
		if(this->determinant() == 0)
		{
			assert(false && "cannot invert this matrix");
			return fmat2x2();
		}

		return (1.0 / this->determinant()) * fmat2x2(
			i22, -i12,
			-i21, i11
		);
	}





	fvec2 operator * (const fmat2x2& m, const fvec2& v)
	{
		return (v.x * m[0]) + (v.y * m[1]);
	}

	fmat2x2 operator * (const fmat2x2& a, const fmat2x2& b)
	{
		fmat2x2 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];

		return ret;
	}

	fmat2x2 operator * (const fmat2x2& a, float s) { return fmat2x2(a.vecs[0] * s, a.vecs[1] * s); }
	fmat2x2 operator * (float s, const fmat2x2& a) { return a * s; }

	fmat2x2 operator / (const fmat2x2& a, float s) { return fmat2x2(a.vecs[0] / s, a.vecs[1] / s); }
	fmat2x2 operator / (float s, const fmat2x2& a) { return a / s; }

	bool operator == (const fmat2x2& a, const fmat2x2& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1]);
	}

	fmat3x3 translate(const fmat2x2& m, const fvec2& v) { return m.translated(v); }
	fmat2x2 rotate(const fmat2x2& m, float rad)         { return m.rotated(rad); }
	fmat2x2 scale(const fmat2x2& m, const fvec2& v)     { return m.scaled(v); }
	fmat2x2 scale(const fmat2x2& m, float d)            { return m.scaled(d); }
	fmat2x2 transpose(const fmat2x2& m)                 { return m.transposed(); }
	fmat2x2 inverse(const fmat2x2& m)					{ return m.inversed(); }
}




















