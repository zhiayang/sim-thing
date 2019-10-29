// lx.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// Linear Algebra Library

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "lx/vec2.h"
#include "lx/vec3.h"
#include "lx/vec4.h"
#include "lx/fvec2.h"
#include "lx/fvec3.h"
#include "lx/fvec4.h"

#include "lx/mat2.h"
#include "lx/mat3.h"
#include "lx/mat4.h"
#include "lx/fmat2.h"
#include "lx/fmat3.h"
#include "lx/fmat4.h"

#include "lx/quat.h"

namespace lx
{
	constexpr double PI = 3.14159265358979323846264338327950;
	constexpr double TAU = 2 * PI;

	float round(float x);
	double round(double x);

	float copysign(float to, float from);
	double copysign(double to, double from);

	float sin(float x);
	float cos(float x);
	float tan(float x);

	double sin(double x);
	double cos(double x);
	double tan(double x);

	float asin(float x);
	float acos(float x);
	float atan(float x);

	double asin(double x);
	double acos(double x);
	double atan(double x);

	float atan2(float y, float x);
	double atan2(double y, double x);

	float abs(float x);
	double abs(double x);

	float pow(float x, float n);
	double pow(double x, double n);

	fvec2 pow(const fvec2& v, float n);
	fvec3 pow(const fvec3& v, float n);
	fvec4 pow(const fvec4& v, float n);

	vec2 pow(const vec2& v, double n);
	vec3 pow(const vec3& v, double n);
	vec4 pow(const vec4& v, double n);

	float toDegrees(float radians);
	float toRadians(float degrees);

	double toDegrees(double radians);
	double toRadians(double degrees);

	vec3 toRadians(const vec3& degrees);
	vec3 toDegrees(const vec3& radians);

	fvec3 toRadians(const fvec3& degrees);
	fvec3 toDegrees(const fvec3& radians);

	float sqrt(float x);
	double sqrt(double x);

	float clamp(float value, float min, float max);
	double clamp(double value, double min, double max);

	vec2 clamp(vec2 value, vec2 min, vec2 max);
	vec3 clamp(vec3 value, vec3 min, vec3 max);
	vec4 clamp(vec4 value, vec4 min, vec4 max);

	fvec2 clamp(fvec2 value, fvec2 min, fvec2 max);
	fvec3 clamp(fvec3 value, fvec3 min, fvec3 max);
	fvec4 clamp(fvec4 value, fvec4 min, fvec4 max);

	mat4x4 lookAt(const vec3& eye, const vec3& centre, const vec3& up);
	mat4x4 perspective(double fov, double aspect, double near, double far);
	mat4x4 orthographic(double left, double right, double bottom, double top);
	mat4x4 orthographic(double left, double right, double bottom, double top, double near, double far);

	mat4x4 translate(const vec3& v);
	mat4x4 scale(const vec3& v);
	mat4x4 scale(double s);

	fmat4x4 translate(const fvec3& v);
	fmat4x4 scale(const fvec3& v);
	fmat4x4 scale(float s);

	float _fastInverseSqrtF(float number);
	double _fastInverseSqrtD(double number);



	fvec2 tof(const vec2& v);
	fvec3 tof(const vec3& v);
	fvec4 tof(const vec4& v);
	fmat2 tof(const mat2& v);
	fmat3 tof(const mat3& v);
	fmat4 tof(const mat4& v);

	std::vector<fvec2> tof(const std::vector<vec2>& vs);
	std::vector<fvec3> tof(const std::vector<vec3>& vs);
	std::vector<fvec4> tof(const std::vector<vec4>& vs);

	vec2 fromf(const fvec2& v);
	vec3 fromf(const fvec3& v);
	vec4 fromf(const fvec4& v);
	mat2 fromf(const fmat2& v);
	mat3 fromf(const fmat3& v);
	mat4 fromf(const fmat4& v);

	std::vector<vec2> fromf(const std::vector<fvec2>& vs);
	std::vector<vec3> fromf(const std::vector<fvec3>& vs);
	std::vector<vec4> fromf(const std::vector<fvec4>& vs);
}


#include <iostream>
namespace tinyformat
{
	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec2& v);
	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec3& v);
	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec4& v);

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec2& v);
	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec3& v);
	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec4& v);

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::quat& v);
}





















