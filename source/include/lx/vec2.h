// vec2.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

namespace lx
{
	struct vec2
	{
		vec2(float a, float b) : x(a), y(b) { }

		explicit vec2() : x(0.0f), y(0.0f) { }
		explicit vec2(float a) : x(a), y(a) { }


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

		vec2& operator += (const vec2& v);
		vec2& operator -= (const vec2& v);

		vec2& operator *= (const vec2& v);
		vec2& operator /= (const vec2& v);

		vec2& operator *= (float s);
		vec2& operator /= (float s);

		float magnitude() const;
		vec2 normalised() const;

		// swizzle
		vec2 xy() const;
		vec2 yx() const;
		vec2 xx() const;
		vec2 yy() const;

		#ifdef AMERICAN_SPELLINGS
			vec2 normalized() const;
		#endif
	};


	vec2 operator + (const vec2& a, const vec2& b);
	vec2 operator - (const vec2& a, const vec2& b);
	vec2 operator * (const vec2& a, const vec2& b);
	vec2 operator / (const vec2& a, const vec2& b);
	bool operator == (const vec2& a, const vec2& b);

	vec2 operator * (const vec2& a, float b);
	vec2 operator / (const vec2& a, float b);
	vec2 operator * (float a, const vec2& b);
	vec2 operator / (float a, const vec2& b);


	vec2 round(const vec2& v);
	vec2 normalise(const vec2& v);
	vec2 normalize(const vec2& v);
	float magnitude(const vec2& v);

	float dot(const vec2& a, const vec2& b);
	float distance(const vec2& a, const vec2& b);
}
















