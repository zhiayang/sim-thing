// fvec2.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "lx/fvec2.h"

#include <assert.h>


namespace lx
{
	float& fvec2::operator[] (size_t i)
	{
		assert(i == 0 || i == 1);
		return this->ptr[i];
	}

	const float& fvec2::operator[] (size_t i) const
	{
		assert(i == 0 || i == 1);
		return this->ptr[i];
	}

	fvec2& fvec2::operator += (const fvec2& v) { this->x += v.x; this->y += v.y; return *this; }
	fvec2& fvec2::operator -= (const fvec2& v) { this->x -= v.x; this->y -= v.y; return *this; }
	fvec2& fvec2::operator *= (const fvec2& v) { this->x *= v.x; this->y *= v.y; return *this; }
	fvec2& fvec2::operator /= (const fvec2& v) { this->x /= v.x; this->y /= v.y; return *this; }

	fvec2& fvec2::operator *= (float s) { this->x *= s; this->y *= s; return *this; }
	fvec2& fvec2::operator /= (float s) { this->x /= s; this->y /= s; return *this; }


	float fvec2::magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y)); }

	fvec2 fvec2::normalised() const
	{
		float mag = (this->x * this->x) + (this->y * this->y);
		float ivs = _fastInverseSqrtF(mag);
		return fvec2(this->x * ivs, this->y * ivs);
	}

	#ifdef AMERICAN_SPELLINGS
		fvec2 fvec2::normalized() const { return this->normalised(); }
	#endif











	fvec2 operator + (const fvec2& a, const fvec2& b) { return fvec2(a.x + b.x, a.y + b.y); }
	fvec2 operator - (const fvec2& a, const fvec2& b) { return fvec2(a.x - b.x, a.y - b.y); }
	fvec2 operator * (const fvec2& a, const fvec2& b) { return fvec2(a.x * b.x, a.y * b.y); }
	fvec2 operator / (const fvec2& a, const fvec2& b) { return fvec2(a.x / b.x, a.y / b.y); }
	bool operator == (const fvec2& a, const fvec2& b) { return a.x == b.x && a.y == b.y; }

	fvec2 operator * (const fvec2& a, float b) { return fvec2(a.x * b, a.y * b); }
	fvec2 operator / (const fvec2& a, float b) { return fvec2(a.x / b, a.y / b); }

	fvec2 operator * (float a, const fvec2& b) { return b * a; }
	fvec2 operator / (float a, const fvec2& b) { return b / a; }

	fvec2 round(const fvec2&v) { return fvec2(lx::round(v.x), lx::round(v.y)); }
	fvec2 normalise(const fvec2& v) { return v.normalised(); }
	fvec2 normalize(const fvec2& v) { return v.normalised(); }
	float magnitude(const fvec2& v) { return v.magnitude(); }

	float dot(const fvec2& a, const fvec2& b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}

	float distance(const fvec2& a, const fvec2& b)
	{
		return lx::sqrt(
			((a.x - b.x) * (a.x - b.x))
			+ ((a.y - b.y) * (a.y - b.y))
		);
	}
}














