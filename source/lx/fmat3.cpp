// fmat3.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include <assert.h>

namespace lx
{
	fmat3x3::fmat3x3()
	{
		// make identity by default
		this->vecs[2] = fvec3(0, 0, 1);
		this->vecs[1] = fvec3(0, 1, 0);
		this->vecs[0] = fvec3(1, 0, 0);
	}

	fmat3x3::fmat3x3(const fvec3& a, const fvec3& b, const fvec3& c)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
		this->vecs[2] = c;
	}

	fmat3x3::fmat3x3(float a, float b, float c,
					float d, float e, float f,
					float g, float h, float i)
	{
		this->vecs[0] = fvec3(a, d, g);
		this->vecs[1] = fvec3(b, e, h);
		this->vecs[2] = fvec3(c, f, i);
	}

	fmat3x3 fmat3x3::identity()
	{
		return fmat3x3();
	}

	fmat3x3 fmat3x3::zero()
	{
		fmat3x3 ret;
		ret.vecs[0] = fvec3(0);
		ret.vecs[1] = fvec3(0);
		ret.vecs[2] = fvec3(0);

		return ret;
	}

	fvec3& fmat3x3::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->vecs[i];
	}

	const fvec3& fmat3x3::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2);
		return this->vecs[i];
	}

	fmat3x3& fmat3x3::operator += (const fmat3x3& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		this->vecs[2] += m.vecs[2];
		return *this;
	}

	fmat3x3& fmat3x3::operator -= (const fmat3x3& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		this->vecs[2] -= m.vecs[2];
		return *this;
	}

	fmat3x3& fmat3x3::operator *= (float s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		this->vecs[2] *= s;
		return *this;
	}

	fmat3x3& fmat3x3::operator /= (float s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		this->vecs[2] /= s;
		return *this;
	}


	fmat4x4 fmat3x3::translated(const fvec3& v) const
	{
		fmat4x4 result;
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

		result.vecs[3] = fvec4(x, y, z, 1);
		result.vecs[2] = fvec4(this->vecs[2], 0);
		result.vecs[1] = fvec4(this->vecs[1], 0);
		result.vecs[0] = fvec4(this->vecs[0], 0);

		return result;
	}

	fmat3x3 fmat3x3::rotated(float radians, const fvec3& axis) const
	{
		float c = lx::cos(radians);
		float s = lx::sin(radians);

		// bad form, i guess
		float C = (1 - c);
		float L = axis.x;
		float M = axis.y;
		float N = axis.z;

		fmat3x3 ret;
		/*
			for axis = (L, M, N)

			v1				v2				v3
			LL(1-c) + c		ML(1-c) - Ns	NL(1-c) + Ms
			LM(1-c) + Ns	MM(1-c) + c		NM(1-c) - Ls
			LN(1-c) - Ms	MN(1-c) + Ls	NN(1-c) + c
		*/

		ret.vecs[2] = fvec3(N * L * C + M * s,	N * M * C - L * s,	N * N * C + c);
		ret.vecs[1] = fvec3(M * L * C - N * s,	M * M * C + c,		M * N * C + L * s);
		ret.vecs[0] = fvec3(L * L * C + c,		L * M * C + N * s,	L * N * C - M * s);

		return ret * (*this);
	}

	fmat3x3 fmat3x3::rotated(const quat& rot) const
	{
		return tof(rot.toRotationMatrix());
	}

	fmat3x3 fmat3x3::scaled(const fvec3& v) const
	{
		fmat3x3 result;
		/*
			v1	v2	v3
			x	0	0
			0	y	0
			0	0	z
		*/
		// result.vecs[2] = fvec3(0, 0, v.z);
		// result.vecs[1] = fvec3(0, v.y, 0);
		// result.vecs[0] = fvec3(v.x, 0, 0);

		result[0] = this->vecs[0] * v[0];
		result[1] = this->vecs[1] * v[1];
		result[2] = this->vecs[2] * v[2];

		return result;
	}

	fmat3x3 fmat3x3::scaled(float s) const
	{
		return this->scaled(fvec3(s));
	}


	fmat3x3 fmat3x3::transposed() const
	{
		/*
			a b c         a d g
			d e f         b e h
			g h i         c f i
		*/
		fmat3x3 result;

		result.vecs[2] = fvec3(this->vecs[0].z, this->vecs[1].z, this->vecs[2].z);
		result.vecs[1] = fvec3(this->vecs[0].y, this->vecs[1].y, this->vecs[2].y);
		result.vecs[0] = fvec3(this->vecs[0].x, this->vecs[1].x, this->vecs[2].x);

		return result;
	}

	float fmat3x3::determinant() const
	{
		return (i11 * i22 * i33)
			+ (i12 * i23 * i31)
			+ (i13 * i21 * i32)
			- (i31 * i22 * i13)
			- (i32 * i23 * i11)
			- (i33 * i21 * i12);
	}

	fmat3x3 fmat3x3::inversed() const
	{
		if(this->determinant() == 0)
		{
			assert(false && "cannot invert this matrix");
			return fmat3x3();
		}

		auto m = *this;

		fmat3 ret;
		auto invdet = 1.0 / this->determinant();
		ret[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
		ret[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
		ret[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
		ret[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
		ret[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
		ret[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
		ret[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
		ret[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
		ret[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

		return ret;
	}








	fvec3 operator * (const fmat3x3& m, const fvec3& v)
	{
		return (v.x * m[0]) + (v.y * m[1]) + (v.z * m[2]);
	}

	fmat3x3 operator * (const fmat3x3& a, const fmat3x3& b)
	{
		fmat3x3 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];
		ret.vecs[2] = a * b.vecs[2];

		return ret;
	}

	fmat3x3 operator * (const fmat3x3& a, float s) { return fmat3x3(a.vecs[0] * s, a.vecs[1] * s, a.vecs[2] * s); }
	fmat3x3 operator * (float s, const fmat3x3& a) { return a * s; }

	fmat3x3 operator / (const fmat3x3& a, float s) { return fmat3x3(a.vecs[0] / s, a.vecs[1] / s, a.vecs[2] / s); }
	fmat3x3 operator / (float s, const fmat3x3& a) { return a / s; }

	bool operator == (const fmat3x3& a, const fmat3x3& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1])
			&& (a.vecs[2] == b.vecs[2]);
	}


	fmat3x3 rotate(const fmat3x3& m, const fvec3& axis, float rad)  { return m.rotated(rad, axis); }
	fmat4x4 translate(const fmat3x3& m, const fvec3& v)             { return m.translated(v); }
	fmat3x3 transpose(const fmat3x3& m)                             { return m.transposed(); }
	fmat3x3 scale(const fmat3x3& m, const fvec3& v)                 { return m.scaled(v); }
	fmat3x3 scale(const fmat3x3& m, float d)                        { return m.scaled(d); }
}




















