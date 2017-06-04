// lx.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// Linear Algebra Library

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "lx/vec2.h"
#include "lx/vec3.h"
#include "lx/vec4.h"

#include "lx/mat2.h"
#include "lx/mat3.h"
#include "lx/mat4.h"

namespace lx
{
	float round(float x);
	double round(double x);

	float sin(float x);
	float cos(float x);
	float tan(float x);

	double sin(double x);
	double cos(double x);
	double tan(double x);

	float toDegrees(float radians);
	float toRadians(float degrees);

	double toDegrees(double radians);
	double toRadians(double degrees);

	float sqrt(float x);
	double sqrt(double x);

	float clamp(float value, float min, float max);
	double clamp(double value, double min, double max);

	vec2 clamp(vec2 value, vec2 min, vec2 max);
	vec3 clamp(vec3 value, vec3 min, vec3 max);
	vec4 clamp(vec4 value, vec4 min, vec4 max);

	mat4x4 lookAt(const vec3& eye, const vec3& centre, const vec3& up);
	mat4x4 perspective(float fov, float aspect, float near, float far);
	mat4x4 orthographic(float left, float right, float bottom, float top);
	mat4x4 orthographic(float left, float right, float bottom, float top, float near, float far);

	mat4x4 translate(const vec3& v);
	mat4x4 scale(const vec3& v);
	mat4x4 scale(float s);
}

























