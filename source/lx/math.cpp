// math.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <math.h>

#include "lx.h"

namespace lx
{
	float round(float x) { return ::roundf(x); }
	double round(double x) { return ::round(x); }

	float abs(float x) { return ::fabs(x); }
	double abs(double x) { return ::fabs(x); }

	float copysign(float to, float from) { return ::copysign(to, from); }
	double copysign(double to, double from) { return ::copysign(to, from); }

	float sin(float x) { return ::sin(x); }
	float cos(float x) { return ::cos(x); }
	float tan(float x) { return ::tan(x); }

	double sin(double x) { return ::sin(x); }
	double cos(double x) { return ::cos(x); }
	double tan(double x) { return ::tan(x); }

	float asin(float x) { return ::asin(x); }
	float acos(float x) { return ::acos(x); }
	float atan(float x) { return ::atan(x); }

	double asin(double x) { return ::asin(x); }
	double acos(double x) { return ::acos(x); }
	double atan(double x) { return ::atan(x); }

	float atan2(float y, float x) { return ::atan2(y, x); }
	double atan2(double y, double x) { return ::atan2(y, x); }

	float pow(float x, float n) { return ::pow(x, n); }
	double pow(double x, double n) { return ::pow(x, n); }

	fvec2 pow(const fvec2& v, float n) { return fvec2(pow(v.x, n), pow(v.y, n)); }
	fvec3 pow(const fvec3& v, float n) { return fvec3(pow(v.x, n), pow(v.y, n), pow(v.z, n)); }
	fvec4 pow(const fvec4& v, float n) { return fvec4(pow(v.x, n), pow(v.y, n), pow(v.z, n), pow(v.w, n)); }

	vec2 pow(const vec2& v, double n) { return vec2(pow(v.x, n), pow(v.y, n)); }
	vec3 pow(const vec3& v, double n) { return vec3(pow(v.x, n), pow(v.y, n), pow(v.z, n)); }
	vec4 pow(const vec4& v, double n) { return vec4(pow(v.x, n), pow(v.y, n), pow(v.z, n), pow(v.w, n)); }


	float toDegrees(float radians) { return radians * 57.295779513082320876798154814105; }
	float toRadians(float degrees) { return degrees * 0.01745329251994329576923690768489; }

	double toDegrees(double radians) { return radians * 57.295779513082320876798154814105; }
	double toRadians(double degrees) { return degrees * 0.01745329251994329576923690768489; }


	vec3 toDegrees(const vec3& v)  { return vec3(toDegrees(v.x), toDegrees(v.y), toDegrees(v.z)); }
	fvec3 toDegrees(const fvec3& v)  { return fvec3(toDegrees(v.x), toDegrees(v.y), toDegrees(v.z)); }

	vec3 toRadians(const vec3& v) { return vec3(toRadians(v.x), toRadians(v.y), toRadians(v.z)); }
	fvec3 toRadians(const fvec3& v)  { return fvec3(toRadians(v.x), toRadians(v.y), toRadians(v.z)); }


	float sqrt(float x) { return ::sqrt(x); }
	double sqrt(double x) { return ::sqrt(x); }
}





