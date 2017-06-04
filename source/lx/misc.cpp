// misc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"

namespace lx
{
	float clamp(float v, float min, float max)
	{
		float ret = (v > max ? max : v);
		return (ret < min ? min : ret);
	}

	double clamp(double v, double min, double max)
	{
		double ret = (v > max ? max : v);
		return (ret < min ? min : ret);
	}

	vec2 clamp(vec2 v, vec2 min, vec2 max)
	{
		return vec2(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y));
	}
	vec3 clamp(vec3 v, vec3 min, vec3 max)
	{
		return vec3(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y), clamp(v.z, min.z, max.z));
	}
	vec4 clamp(vec4 v, vec4 min, vec4 max)
	{
		return vec4(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y), clamp(v.z, min.z, max.z), clamp(v.w, min.w, max.w));
	}




	mat4x4 translate(const vec3& v)
	{
		return mat4().translate(v);
	}

	mat4x4 scale(const vec3& v)
	{
		return mat4().scale(v);
	}

	mat4x4 scale(float s)
	{
		return mat4().scale(s);
	}







	mat4 perspective(float fov, float aspect, float near, float far)
	{
		float tanHalfFov = lx::tan(fov / 2.0);
		auto result = mat4::zero();

		result[0][0] = 1.0 / (aspect * tanHalfFov);
		result[1][1] = 1.0 / (tanHalfFov);
		result[2][3] = -1.0;

		result[2][2] = -(far + near) / (far - near);
		result[3][2] = -(2.0 * far * near) / (far - near);

		return result;
	}

	mat4 orthographic(float left, float right, float bottom, float top)
	{
		mat4 result;

		result[0][0] = 2.0 / (right - left);
		result[1][1] = 2.0 / (top - bottom);
		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);

		result[2][2] = -1;

		return result;
	}

	mat4 orthographic(float left, float right, float bottom, float top, float near, float far)
	{
		mat4 result;

		result[0][0] = 2.0 / (right - left);
		result[1][1] = 2.0 / (top - bottom);
		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);

		result[2][2] = -2.0 / (far - near);
		result[3][2] = -(far + near) / (far - near);

		return result;
	}


	mat4 lookAt(const vec3& eye, const vec3& centre, const vec3& up)
	{
		auto f = (centre - eye).normalised();
		auto s = cross(f, up).normalised();
		auto u = cross(s, f);

		mat4 result;
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] = -f.x;
		result[1][2] = -f.y;
		result[2][2] = -f.z;
		result[3][0] = -dot(s, eye);
		result[3][1] = -dot(u, eye);
		result[3][2] = dot(f, eye);

		return result;


		/*

		vec<3, T, P> const f(normalize(center - eye));
		vec<3, T, P> const s(normalize(cross(f, up)));
		vec<3, T, P> const u(cross(s, f));

		mat<4, 4, T, P> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] =-f.x;
		Result[1][2] =-f.y;
		Result[2][2] =-f.z;
		Result[3][0] =-dot(s, eye);
		Result[3][1] =-dot(u, eye);
		Result[3][2] = dot(f, eye);
		return Result;*/
	}
}
