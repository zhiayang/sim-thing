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

	float atan2(float x, float y) { return ::atan2(x, y); }
	double atan2(double x, double y) { return ::atan2(x, y); }


	float toDegrees(float radians) { return radians * 57.295779513082320876798154814105; }
	float toRadians(float degrees) { return degrees * 0.01745329251994329576923690768489; }

	double toDegrees(double radians) { return radians * 57.295779513082320876798154814105; }
	double toRadians(double degrees) { return degrees * 0.01745329251994329576923690768489; }

	float sqrt(float x) { return ::sqrt(x); }
	double sqrt(double x) { return ::sqrt(x); }
}
