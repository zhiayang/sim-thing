// fmat4.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "lx.h"
#include <assert.h>

namespace lx
{
	fmat4x4::fmat4x4()
	{
		// make identity by default
		this->vecs[0] = fvec4(1, 0, 0, 0);
		this->vecs[1] = fvec4(0, 1, 0, 0);
		this->vecs[2] = fvec4(0, 0, 1, 0);
		this->vecs[3] = fvec4(0, 0, 0, 1);
	}

	fmat4x4::fmat4x4(const fvec4& a, const fvec4& b, const fvec4& c, const fvec4& d)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
		this->vecs[2] = c;
		this->vecs[3] = d;
	}

	fmat4x4::fmat4x4(float a, float b, float c, float d,
					float e, float f, float g, float h,
					float i, float j, float k, float l,
					float m, float n, float o, float p)
	{
		this->vecs[0] = fvec4(a, e, i, m);
		this->vecs[1] = fvec4(b, f, j, n);
		this->vecs[2] = fvec4(c, g, k, o);
		this->vecs[3] = fvec4(d, h, l, p);
	}


	fmat4x4 fmat4x4::identity()
	{
		return fmat4x4();
	}

	fmat4x4 fmat4x4::zero()
	{
		fmat4x4 ret;
		ret.vecs[0] = fvec4(0);
		ret.vecs[1] = fvec4(0);
		ret.vecs[2] = fvec4(0);
		ret.vecs[3] = fvec4(0);

		return ret;
	}

	fvec4& fmat4x4::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->vecs[i];
	}

	const fvec4& fmat4x4::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->vecs[i];
	}

	fmat4x4& fmat4x4::operator += (const fmat4x4& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		this->vecs[2] += m.vecs[2];
		this->vecs[3] += m.vecs[3];
		return *this;
	}

	fmat4x4& fmat4x4::operator -= (const fmat4x4& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		this->vecs[2] -= m.vecs[2];
		this->vecs[3] -= m.vecs[3];
		return *this;
	}

	fmat4x4& fmat4x4::operator *= (float s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		this->vecs[2] *= s;
		this->vecs[3] *= s;
		return *this;
	}

	fmat4x4& fmat4x4::operator /= (float s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		this->vecs[2] /= s;
		this->vecs[3] /= s;
		return *this;
	}


	fmat4x4 fmat4x4::translated(const fvec3& v) const
	{
		fmat4x4 result = *this;
		result[3] = this->vecs[0] * v[0] + this->vecs[1] * v[1] + this->vecs[2] * v[2] + this->vecs[3];

		return result;
	}

	fmat4x4 fmat4x4::rotated(float radians, const fvec3& axis) const
	{
		float c = lx::cos(radians);
		float s = lx::sin(radians);

		// bad form, i guess
		float C = (1 - c);
		float L = axis.x;
		float M = axis.y;
		float N = axis.z;

		fmat4x4 ret;
		/*
			for axis = (L, M, N)

			v1				v2				v3				v4
			LL(1-c) + c		ML(1-c) - Ns	NL(1-c) + Ms	0
			LM(1-c) + Ns	MM(1-c) + c		NM(1-c) - Ls	0
			LN(1-c) - Ms	MN(1-c) + Ls	NN(1-c) + c		0
			0				0				0				1
		*/

		ret.vecs[3] = fvec4(0,                  0,                  0,                  1);
		ret.vecs[2] = fvec4(N * L * C + M * s,  N * M * C - L * s,  N * N * C + c,      0);
		ret.vecs[1] = fvec4(M * L * C - N * s,  M * M * C + c,      M * N * C + L * s,  0);
		ret.vecs[0] = fvec4(L * L * C + c,      L * M * C + N * s,  L * N * C - M * s,  0);

		return ret * (*this);
	}


	fmat4x4 fmat4x4::rotated(const quat& rot) const
	{
		return this->rotated(rot.angle(), tof(rot.axis()));
	}


	fmat4x4 fmat4x4::rotationOnly() const
	{
		fmat4x4 result;

		result[3] = fvec4(0,     0,     0,     1);
		result[2] = fvec4(this->vecs[2].xyz(), 0);
		result[1] = fvec4(this->vecs[1].xyz(), 0);
		result[0] = fvec4(this->vecs[0].xyz(), 0);

		return result;
	}



	fmat4x4 fmat4x4::scaled(const fvec3& v) const
	{
		fmat4x4 result;
		/*
			v1	v2	v3	v4
			x	0	0	0
			0	y	0	0
			0	0	z	0
			0	0	0	1
		*/

		result[0] = this->vecs[0] * v[0];
		result[1] = this->vecs[1] * v[1];
		result[2] = this->vecs[2] * v[2];
		result[3] = this->vecs[3];

		return result;
	}

	fmat4x4 fmat4x4::scaled(float s) const
	{
		return this->scaled(fvec3(s));
	}


	fmat4x4 fmat4x4::transposed() const
	{
		fmat4x4 result;

		/*
			a b c d        a e i m
			e f g h        b f j n
			i j k l        c g k o
			m n o p        d h l p
		*/

		result[3] = fvec4(this->vecs[0].w, this->vecs[1].w, this->vecs[2].w, this->vecs[3].w);
		result[2] = fvec4(this->vecs[0].z, this->vecs[1].z, this->vecs[2].z, this->vecs[3].z);
		result[1] = fvec4(this->vecs[0].y, this->vecs[1].y, this->vecs[2].y, this->vecs[3].y);
		result[0] = fvec4(this->vecs[0].x, this->vecs[1].x, this->vecs[2].x, this->vecs[3].x);

		return result;
	}

	fmat3x3 fmat4x4::upper3x3() const
	{
		fmat3x3 result;
		result[0] = this->vecs[0].xyz();
		result[1] = this->vecs[1].xyz();
		result[2] = this->vecs[2].xyz();

		return result;
	}










	fvec4 operator * (const fmat4x4& m, const fvec4& v)
	{
		return (v.x * m[0]) + (v.y * m[1]) + (v.z * m[2]) + (v.w * m[3]);
	}

	fmat4x4 operator * (const fmat4x4& a, const fmat4x4& b)
	{
		fmat4x4 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];
		ret.vecs[2] = a * b.vecs[2];
		ret.vecs[3] = a * b.vecs[3];

		return ret;
	}

	fmat4x4 operator * (const fmat4x4& a, float s) { return fmat4x4(a.vecs[0] * s, a.vecs[1] * s, a.vecs[2] * s, a.vecs[3] * s); }
	fmat4x4 operator * (float s, const fmat4x4& a) { return a * s; }

	fmat4x4 operator / (const fmat4x4& a, float s) { return fmat4x4(a.vecs[0] / s, a.vecs[1] / s, a.vecs[2] / s, a.vecs[3] * s); }
	fmat4x4 operator / (float s, const fmat4x4& a) { return a / s; }

	bool operator == (const fmat4x4& a, const fmat4x4& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1])
			&& (a.vecs[2] == b.vecs[2])
			&& (a.vecs[3] == b.vecs[3]);
	}

	fmat4x4 rotate(const fmat4x4& m, const fvec3& axis, float rad)  { return m.rotated(rad, axis); }
	fmat4x4 translate(const fmat4x4& m, const fvec3& v)             { return m.translated(v); }
	fmat4x4 transpose(const fmat4x4& m)                             { return m.transposed(); }
	fmat4x4 scale(const fmat4x4& m, const fvec3& v)                 { return m.scaled(v); }
	fmat4x4 scale(const fmat4x4& m, float d)                        { return m.scaled(d); }
}












