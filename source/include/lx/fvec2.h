// fvec2.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

namespace lx
{
	struct fvec2
	{
		fvec2(float a, float b) : x(a), y(b) { }

		fvec2() : x(0.0f), y(0.0f) { }
		explicit fvec2(float a) : x(a), y(a) { }


		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			float ptr[0];
			struct { float x; float y; };
			struct { float u; float v; };
		};
		#pragma clang diagnostic pop

		float& operator[] (size_t i);
		const float& operator[] (size_t i) const;
		fvec2 operator - () const;

		fvec2& operator += (const fvec2& v);
		fvec2& operator -= (const fvec2& v);

		fvec2& operator *= (const fvec2& v);
		fvec2& operator /= (const fvec2& v);

		fvec2& operator *= (float s);
		fvec2& operator /= (float s);



		float magnitude() const;
		fvec2 normalised() const;

		// swizzle
		fvec2 xy() const;
		fvec2 yx() const;
		fvec2 xx() const;
		fvec2 yy() const;
	};


	fvec2 operator + (const fvec2& a, const fvec2& b);
	fvec2 operator - (const fvec2& a, const fvec2& b);
	fvec2 operator * (const fvec2& a, const fvec2& b);
	fvec2 operator / (const fvec2& a, const fvec2& b);
	bool operator == (const fvec2& a, const fvec2& b);

	fvec2 operator * (const fvec2& a, float b);
	fvec2 operator / (const fvec2& a, float b);
	fvec2 operator * (float a, const fvec2& b);
	fvec2 operator / (float a, const fvec2& b);


	fvec2 round(const fvec2& v);
	fvec2 normalise(const fvec2& v);
	float magnitude(const fvec2& v);

	float dot(const fvec2& a, const fvec2& b);
	float distance(const fvec2& a, const fvec2& b);
}
















