// fvec4.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/fvec2.h"
#include "lx/fvec3.h"

namespace lx
{
	struct fvec4
	{
		fvec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) { }

		explicit fvec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
		explicit fvec4(float a) : x(a), y(a), z(a), w(a) { }
		explicit fvec4(const fvec2& v2, float c, float d) : x(v2.x), y(v2.y), z(c), w(d) { }
		explicit fvec4(const fvec3& v3, float d) : x(v3.x), y(v3.y), z(v3.z), w(d) { }

		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			struct { float x; float y; float z; float w; };
			struct { float r; float g; float b; float a; };
		};
		#pragma clang diagnostic pop

		float& operator[] (size_t i);
		const float& operator[] (size_t i) const;
		fvec4& operator += (const fvec4& v);
		fvec4& operator -= (const fvec4& v);
		fvec4& operator *= (const fvec4& v);
		fvec4& operator /= (const fvec4& v);

		fvec4& operator *= (float s);
		fvec4& operator /= (float s);

		float magnitude() const;
		fvec4 normalised() const;

		// dammit. 24???
		fvec2 xy() const;
		fvec2 yx() const;

		fvec3 xyz() const;
		fvec3 xzy() const;
		fvec3 yxz() const;
		fvec3 yzx() const;
		fvec3 zxy() const;
		fvec3 zyx() const;

		fvec3 rgb() const;
		fvec3 bgr() const;

		fvec4 xyzw() const;
		fvec4 xywz() const;
		fvec4 xzyw() const;
		fvec4 xzwy() const;
		fvec4 xwyz() const;
		fvec4 xwzy() const;
		fvec4 yxwz() const;
		fvec4 yxzw() const;
		fvec4 yzwx() const;
		fvec4 yzxw() const;
		fvec4 ywzx() const;
		fvec4 ywxz() const;
		fvec4 zxyw() const;
		fvec4 zxwy() const;
		fvec4 zyxw() const;
		fvec4 zywx() const;
		fvec4 zwxy() const;
		fvec4 zwyx() const;
		fvec4 wxzy() const;
		fvec4 wxyz() const;
		fvec4 wyzx() const;
		fvec4 wyxz() const;
		fvec4 wzyx() const;
		fvec4 wzxy() const;

		fvec4 rgba() const;
		fvec4 bgra() const;
		fvec4 argb() const;
		fvec4 abgr() const;

		fvec2 xx() const;
		fvec2 yy() const;
		fvec2 zz() const;
		fvec2 ww() const;

		fvec3 xxx() const;
		fvec3 yyy() const;
		fvec3 zzz() const;
		fvec3 www() const;

		fvec4 xxxx() const;
		fvec4 yyyy() const;
		fvec4 zzzz() const;
		fvec4 wwww() const;


		#ifdef AMERICAN_SPELLINGS
			fvec4 normalized() const;
		#endif
	};


	fvec4 operator + (const fvec4& a, const fvec4& b);
	fvec4 operator - (const fvec4& a, const fvec4& b);
	fvec4 operator * (const fvec4& a, const fvec4& b);
	fvec4 operator / (const fvec4& a, const fvec4& b);
	bool operator == (const fvec4& a, const fvec4& b);

	fvec4 operator * (const fvec4& a, float b);
	fvec4 operator / (const fvec4& a, float b);
	fvec4 operator * (float a, const fvec4& b);
	fvec4 operator / (float a, const fvec4& b);

	fvec4 round(const fvec4& v);
	fvec4 normalise(const fvec4& v);
	fvec4 normalize(const fvec4& v);
	float magnitude(const fvec4& v);

	float dot(const fvec4& a, const fvec4& b);
	float distance(const fvec4& a, const fvec4& b);
}
















