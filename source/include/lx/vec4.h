// vec4.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec2.h"
#include "lx/vec3.h"

namespace lx
{
	struct vec4
	{
		vec4(double a, double b, double c, double d) : x(a), y(b), z(c), w(d) { }

		explicit vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
		explicit vec4(double a) : x(a), y(a), z(a), w(a) { }
		explicit vec4(const vec2& v2, double c, double d) : x(v2.x), y(v2.y), z(c), w(d) { }
		explicit vec4(const vec3& v3, double d) : x(v3.x), y(v3.y), z(v3.z), w(d) { }

		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			double ptr[0];
			struct { double x; double y; double z; double w; };
			struct { double r; double g; double b; double a; };
		};
		#pragma clang diagnostic pop

		double& operator[] (size_t i);
		const double& operator[] (size_t i) const;
		vec4& operator += (const vec4& v);
		vec4& operator -= (const vec4& v);
		vec4& operator *= (const vec4& v);
		vec4& operator /= (const vec4& v);

		vec4& operator *= (double s);
		vec4& operator /= (double s);

		double magnitude() const;
		vec4 normalised() const;

		// dammit. 24???
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

		vec4 xyzw() const;
		vec4 xywz() const;
		vec4 xzyw() const;
		vec4 xzwy() const;
		vec4 xwyz() const;
		vec4 xwzy() const;
		vec4 yxwz() const;
		vec4 yxzw() const;
		vec4 yzwx() const;
		vec4 yzxw() const;
		vec4 ywzx() const;
		vec4 ywxz() const;
		vec4 zxyw() const;
		vec4 zxwy() const;
		vec4 zyxw() const;
		vec4 zywx() const;
		vec4 zwxy() const;
		vec4 zwyx() const;
		vec4 wxzy() const;
		vec4 wxyz() const;
		vec4 wyzx() const;
		vec4 wyxz() const;
		vec4 wzyx() const;
		vec4 wzxy() const;

		vec4 rgba() const;
		vec4 bgra() const;
		vec4 argb() const;
		vec4 abgr() const;

		vec2 xx() const;
		vec2 yy() const;
		vec2 zz() const;
		vec2 ww() const;

		vec3 xxx() const;
		vec3 yyy() const;
		vec3 zzz() const;
		vec3 www() const;

		vec4 xxxx() const;
		vec4 yyyy() const;
		vec4 zzzz() const;
		vec4 wwww() const;


		#ifdef AMERICAN_SPELLINGS
			vec4 normalized() const;
		#endif
	};


	vec4 operator + (const vec4& a, const vec4& b);
	vec4 operator - (const vec4& a, const vec4& b);
	vec4 operator * (const vec4& a, const vec4& b);
	vec4 operator / (const vec4& a, const vec4& b);
	bool operator == (const vec4& a, const vec4& b);

	vec4 operator * (const vec4& a, double b);
	vec4 operator / (const vec4& a, double b);
	vec4 operator * (double a, const vec4& b);
	vec4 operator / (double a, const vec4& b);

	vec4 round(const vec4& v);
	vec4 normalise(const vec4& v);
	vec4 normalize(const vec4& v);
	double magnitude(const vec4& v);

	double dot(const vec4& a, const vec4& b);
	double distance(const vec4& a, const vec4& b);
}
















