// vectors.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include <math.h>

namespace lx
{
	static float _fastInverseSqrt(float number)
	{
		int32_t i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = *(long*) &y;						// evil floating point bit level hacking
		i  = 0x5f3759df - (i >> 1);				// what the fuck?
		y  = * (float*) &i;
		y  = y * (threehalfs - (x2 * y * y));	// 1st iteration
		y  = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed

		return y;
	}



	struct vec2
	{
		vec2() : x(0.0f), y(0.0f) { }
		vec2(float a) : x(a), y(a) { }
		vec2(float a, float b) : x(a), y(b) { }

		union
		{
			float ptr[0];
			struct
			{
				float x;
				float y;
			};

			struct
			{
				float u;
				float v;
			};
		};

		float& operator[] (size_t i) { return this->ptr[i]; }
		const float& operator[] (size_t i) const { return this->ptr[i]; }

		float magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y)); }

		vec2 normalised() const
		{
			float mag = (this->x * this->x) + (this->y * this->y);
			float ivs = _fastInverseSqrt(mag);
			return vec2(this->x * ivs, this->y * ivs);
		}

		vec2 normalized() const { return this->normalised(); }
	};

	struct vec3
	{
		vec3() : x(0.0f), y(0.0f), z(0.0f) { }
		vec3(float a) : x(a), y(a), z(a) { }
		vec3(float a, float b, float c) : x(a), y(b), z(c) { }

		union
		{
			float ptr[0];
			struct
			{
				float x;
				float y;
				float z;
			};

			struct
			{
				float r;
				float g;
				float b;
			};

			struct
			{
				float u;
				float v;
				float w;
			};
		};

		float& operator[] (size_t i) { return this->ptr[i]; }
		const float& operator[] (size_t i) const { return this->ptr[i]; }

		float magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }

		vec3 normalised() const
		{
			float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
			float ivs = _fastInverseSqrt(mag);
			return vec3(this->x * ivs, this->y * ivs, this->z * ivs);
		}

		vec3 normalized() const { return this->normalised(); }
	};

	struct vec4
	{
		vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
		vec4(float a) : x(a), y(a), z(a), w(a) { }
		vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) { }

		union
		{
			float ptr[0];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
		};

		float& operator[] (size_t i) { return this->ptr[i]; }
		const float& operator[] (size_t i) const { return this->ptr[i]; }

		float magnitude() const { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z) * (this->w * this->w)); }

		vec4 normalised() const
		{
			float mag = (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
			float ivs = _fastInverseSqrt(mag);
			return vec4(this->x * ivs, this->y * ivs, this->z * ivs, this->w * ivs);
		}

		vec4 normalized() const { return this->normalised(); }
	};



	vec2 operator + (const vec2& a, const vec2& b) { return vec2(a.x + b.x, a.y + b.y); }
	vec2 operator - (const vec2& a, const vec2& b) { return vec2(a.x - b.x, a.y - b.y); }
	vec2 operator * (const vec2& a, const vec2& b) { return vec2(a.x * b.x, a.y * b.y); }
	vec2 operator / (const vec2& a, const vec2& b) { return vec2(a.x / b.x, a.y / b.y); }

	vec2 normalise(const vec2& v) { return v.normalised(); }
	vec2 normalize(const vec2& v) { return v.normalised(); }
	vec2 magnitude(const vec2& v) { return v.magnitude(); }



	vec3 operator + (const vec3& a, const vec3& b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
	vec3 operator - (const vec3& a, const vec3& b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
	vec3 operator * (const vec3& a, const vec3& b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
	vec3 operator / (const vec3& a, const vec3& b) { return vec3(a.x / b.x, a.y / b.y, a.z / b.z); }

	vec3 normalise(const vec3& v) { return v.normalised(); }
	vec3 normalize(const vec3& v) { return v.normalised(); }
	vec3 magnitude(const vec3& v) { return v.magnitude(); }




	vec4 operator + (const vec4& a, const vec4& b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	vec4 operator - (const vec4& a, const vec4& b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	vec4 operator * (const vec4& a, const vec4& b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	vec4 operator / (const vec4& a, const vec4& b) { return vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }

	vec4 normalise(const vec4& v) { return v.normalised(); }
	vec4 normalize(const vec4& v) { return v.normalised(); }
	vec4 magnitude(const vec4& v) { return v.magnitude(); }
}
















