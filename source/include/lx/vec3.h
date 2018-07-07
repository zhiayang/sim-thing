// vec3.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec2.h"

namespace lx
{
	struct vec3
	{
		vec3(double a, double b, double c) : x(a), y(b), z(c) { }

		explicit vec3() : x(0.0f), y(0.0f), z(0.0f) { }
		explicit vec3(double a) : x(a), y(a), z(a) { }
		explicit vec3(const vec2& v2, double c) : x(v2.x), y(v2.y), z(c) { }


		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			double ptr[0];
			struct { double x; double y; double z; };
			struct { double r; double g; double b; };
			struct { double s; double t; double u; };
		};
		#pragma clang diagnostic pop

		double& operator[] (size_t i);
		const double& operator[] (size_t i) const;
		vec3& operator += (const vec3& v);
		vec3& operator -= (const vec3& v);
		vec3& operator *= (const vec3& v);
		vec3& operator /= (const vec3& v);

		vec3& operator *= (double s);
		vec3& operator /= (double s);

		double magnitude() const;
		vec3 normalised() const;


		// swizzling
		vec2 xy() const;
		vec2 yx() const;

		vec3 xyz() const;
		vec3 xzy() const;
		vec3 yxz() const;
		vec3 yzx() const;
		vec3 zxy() const;
		vec3 zyx() const;

		vec3 rgb() const;
		vec3 bgr() const;

		vec2 xx() const;
		vec2 yy() const;
		vec2 zz() const;
		vec3 xxx() const;
		vec3 yyy() const;
		vec3 zzz() const;


		#ifdef AMERICAN_SPELLINGS
			vec3 normalized() const;
		#endif
	};

	vec3 operator + (const vec3& a, const vec3& b);
	vec3 operator - (const vec3& a, const vec3& b);
	vec3 operator * (const vec3& a, const vec3& b);
	vec3 operator / (const vec3& a, const vec3& b);
	bool operator == (const vec3& a, const vec3& b);

	vec3 operator * (const vec3& a, double b);
	vec3 operator / (const vec3& a, double b);
	vec3 operator * (double a, const vec3& b);
	vec3 operator / (double a, const vec3& b);

	vec3 round(const vec3& v);
	vec3 normalise(const vec3& v);
	vec3 normalize(const vec3& v);
	double magnitude(const vec3& v);
	vec3 cross(const vec3& a, const vec3& b);

	double dot(const vec3& a, const vec3& b);
	double distance(const vec3& a, const vec3& b);
}
















