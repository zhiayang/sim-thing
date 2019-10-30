// mat4.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "lx.h"
#include <assert.h>

namespace lx
{
	mat4x4::mat4x4()
	{
		// make identity by default
		this->vecs[0] = vec4(1, 0, 0, 0);
		this->vecs[1] = vec4(0, 1, 0, 0);
		this->vecs[2] = vec4(0, 0, 1, 0);
		this->vecs[3] = vec4(0, 0, 0, 1);
	}

	mat4x4::mat4x4(const vec4& a, const vec4& b, const vec4& c, const vec4& d)
	{
		this->vecs[0] = a;
		this->vecs[1] = b;
		this->vecs[2] = c;
		this->vecs[3] = d;
	}

	mat4x4::mat4x4(double a, double b, double c, double d,
					double e, double f, double g, double h,
					double i, double j, double k, double l,
					double m, double n, double o, double p)
	{
		this->vecs[0] = vec4(a, e, i, m);
		this->vecs[1] = vec4(b, f, j, n);
		this->vecs[2] = vec4(c, g, k, o);
		this->vecs[3] = vec4(d, h, l, p);
	}


	mat4x4 mat4x4::identity()
	{
		return mat4x4();
	}

	mat4x4 mat4x4::zero()
	{
		mat4x4 ret;
		ret.vecs[0] = vec4(0);
		ret.vecs[1] = vec4(0);
		ret.vecs[2] = vec4(0);
		ret.vecs[3] = vec4(0);

		return ret;
	}

	vec4& mat4x4::operator[] (size_t i)
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->vecs[i];
	}

	const vec4& mat4x4::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1 || i == 2 || i == 3);
		return this->vecs[i];
	}

	mat4x4& mat4x4::operator += (const mat4x4& m)
	{
		this->vecs[0] += m.vecs[0];
		this->vecs[1] += m.vecs[1];
		this->vecs[2] += m.vecs[2];
		this->vecs[3] += m.vecs[3];
		return *this;
	}

	mat4x4& mat4x4::operator -= (const mat4x4& m)
	{
		this->vecs[0] -= m.vecs[0];
		this->vecs[1] -= m.vecs[1];
		this->vecs[2] -= m.vecs[2];
		this->vecs[3] -= m.vecs[3];
		return *this;
	}

	mat4x4& mat4x4::operator *= (double s)
	{
		this->vecs[0] *= s;
		this->vecs[1] *= s;
		this->vecs[2] *= s;
		this->vecs[3] *= s;
		return *this;
	}

	mat4x4& mat4x4::operator /= (double s)
	{
		this->vecs[0] /= s;
		this->vecs[1] /= s;
		this->vecs[2] /= s;
		this->vecs[3] /= s;
		return *this;
	}


	mat4x4 mat4x4::translated(const vec3& v) const
	{
		mat4x4 result = *this;
		result[3] = this->vecs[0] * v[0] + this->vecs[1] * v[1] + this->vecs[2] * v[2] + this->vecs[3];

		return result;
	}

	mat4x4 mat4x4::rotated(double radians, const vec3& axis) const
	{
		double c = lx::cos(radians);
		double s = lx::sin(radians);

		// bad form, i guess
		double C = (1 - c);
		double L = axis.x;
		double M = axis.y;
		double N = axis.z;

		mat4x4 ret;
		/*
			for axis = (L, M, N)

			v1				v2				v3				v4
			LL(1-c) + c		ML(1-c) - Ns	NL(1-c) + Ms	0
			LM(1-c) + Ns	MM(1-c) + c		NM(1-c) - Ls	0
			LN(1-c) - Ms	MN(1-c) + Ls	NN(1-c) + c		0
			0				0				0				1
		*/

		// stolen from https://github.com/g-truc/glm/blob/master/glm/gtx/matrix_interpolation.inl
		ret.vecs[3] = vec4(0,                   0,                  0,                  1);
		ret.vecs[2] = vec4(N * L * C + M * s,   N * M * C - L * s,  N * N * C + c,      0);
		ret.vecs[1] = vec4(M * L * C - N * s,   M * M * C + c,      M * N * C + L * s,  0);
		ret.vecs[0] = vec4(L * L * C + c,       L * M * C + N * s,  L * N * C - M * s,  0);

		return (*this) * ret;
	}

	mat4x4 mat4x4::rotationOnly() const
	{
		mat4x4 result;

		result[3] = vec4(0,     0,     0,     1);
		result[2] = vec4(this->vecs[2].xyz(), 0);
		result[1] = vec4(this->vecs[1].xyz(), 0);
		result[0] = vec4(this->vecs[0].xyz(), 0);

		return result;
	}



	mat4x4 mat4x4::scaled(const vec3& v) const
	{
		mat4x4 result;
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

	mat4x4 mat4x4::scaled(double s) const
	{
		return this->scaled(vec3(s));
	}

	mat4x4 mat4x4::transposed() const
	{
		mat4x4 result;

		/*
			a b c d        a e i m
			e f g h        b f j n
			i j k l        c g k o
			m n o p        d h l p
		*/

		result[3] = vec4(this->vecs[0].w, this->vecs[1].w, this->vecs[2].w, this->vecs[3].w);
		result[2] = vec4(this->vecs[0].z, this->vecs[1].z, this->vecs[2].z, this->vecs[3].z);
		result[1] = vec4(this->vecs[0].y, this->vecs[1].y, this->vecs[2].y, this->vecs[3].y);
		result[0] = vec4(this->vecs[0].x, this->vecs[1].x, this->vecs[2].x, this->vecs[3].x);

		return result;
	}

	mat3x3 mat4x4::upper3x3() const
	{
		mat3x3 result;
		result[0] = this->vecs[0].xyz();
		result[1] = this->vecs[1].xyz();
		result[2] = this->vecs[2].xyz();

		return result;
	}



	vec4 operator * (const mat4x4& m, const vec4& v)
	{
		return (v.x * m[0]) + (v.y * m[1]) + (v.z * m[2]) + (v.w * m[3]);
	}

	mat4x4 operator * (const mat4x4& a, const mat4x4& b)
	{
		mat4x4 ret;
		ret.vecs[0] = a * b.vecs[0];
		ret.vecs[1] = a * b.vecs[1];
		ret.vecs[2] = a * b.vecs[2];
		ret.vecs[3] = a * b.vecs[3];

		return ret;
	}

	mat4x4 operator * (const mat4x4& a, double s) { return mat4x4(a.vecs[0] * s, a.vecs[1] * s, a.vecs[2] * s, a.vecs[3] * s); }
	mat4x4 operator * (double s, const mat4x4& a) { return a * s; }

	mat4x4 operator / (const mat4x4& a, double s) { return mat4x4(a.vecs[0] / s, a.vecs[1] / s, a.vecs[2] / s, a.vecs[3] * s); }
	mat4x4 operator / (double s, const mat4x4& a) { return a / s; }

	bool operator == (const mat4x4& a, const mat4x4& b)
	{
		return (a.vecs[0] == b.vecs[0])
			&& (a.vecs[1] == b.vecs[1])
			&& (a.vecs[2] == b.vecs[2])
			&& (a.vecs[3] == b.vecs[3]);
	}

	mat4x4 rotate(const mat4x4& m, const vec3& axis, double rad)    { return m.rotated(rad, axis); }
	mat4x4 translate(const mat4x4& m, const vec3& v)                { return m.translated(v); }
	mat4x4 scale(const mat4x4& m, const vec3& v)                    { return m.scaled(v); }
	mat4x4 scale(const mat4x4& m, double d)                         { return m.scaled(d); }
	mat4x4 transpose(const mat4x4& m)                               { return m.transposed(); }
}












