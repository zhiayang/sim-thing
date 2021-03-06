// misc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"
#include "utilities.h"

namespace lx
{
	float _fastInverseSqrtF(float number)
	{
		int32_t i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = *(int32_t*) &y;					// evil floating point bit level hacking
		i  = 0x5F3759DF - (i >> 1);				// what the fuck?
		y  = *(float*) &i;
		y  = y * (threehalfs - (x2 * y * y));	// 1st iteration
		y  = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed

		return y;
	}

	double _fastInverseSqrtD(double number)
	{
		double y = number;
		double x2 = y * 0.5;

		int64_t i = *((int64_t*) &y);

		// The magic number is for doubles is from https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf

		i = 0x5fe6eb50c7b537a9 - (i >> 1);
		y = *((double*) &i);

		y = y * (1.5 - (x2 * y * y));		// 1st iteration
		y = y * (1.5 - (x2 * y * y));		// 2nd iteration, this can be removed

		return y;
	}




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


	fvec2 clamp(fvec2 v, fvec2 min, fvec2 max)
	{
		return fvec2(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y));
	}
	fvec3 clamp(fvec3 v, fvec3 min, fvec3 max)
	{
		return fvec3(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y), clamp(v.z, min.z, max.z));
	}
	fvec4 clamp(fvec4 v, fvec4 min, fvec4 max)
	{
		return fvec4(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y), clamp(v.z, min.z, max.z), clamp(v.w, min.w, max.w));
	}



	mat4x4 translate(const vec3& v)		{ return mat4().translated(v); }
	fmat4x4 translate(const fvec3& v)	{ return fmat4().translated(v); }

	mat4x4 scale(const vec3& v)			{ return mat4().scaled(v); }
	fmat4x4 scale(const fvec3& v)		{ return fmat4().scaled(v); }

	mat4x4 scale(double s)				{ return mat4().scaled(s); }
	fmat4x4 scale(float s)				{ return fmat4().scaled(s); }



	template <typename T>
	static T _lerp(const T& v0, const T& v1, double t)
	{
		t = clamp(t, 0, 1);
		return (1 - t) * v0 + t * v1;
	}

	float lerp(float v0, float v1, double t)                { return _lerp(v0, v1, t); }
	double lerp(double v0, double v1, double t)             { return _lerp(v0, v1, t); }
	vec2 lerp(const vec2& v0, const vec2& v1, double t)     { return _lerp(v0, v1, t); }
	vec3 lerp(const vec3& v0, const vec3& v1, double t)     { return _lerp(v0, v1, t); }
	vec4 lerp(const vec4& v0, const vec4& v1, double t)     { return _lerp(v0, v1, t); }
	fvec2 lerp(const fvec2& v0, const fvec2& v1, double t)  { return _lerp(v0, v1, t); }
	fvec3 lerp(const fvec3& v0, const fvec3& v1, double t)  { return _lerp(v0, v1, t); }
	fvec4 lerp(const fvec4& v0, const fvec4& v1, double t)  { return _lerp(v0, v1, t); }




	mat4 perspective(double fov, double aspect, double near, double far)
	{
		double tanHalfFov = lx::tan(fov / 2.0);
		auto result = mat4::zero();

		result[0][0] = 1.0 / (aspect * tanHalfFov);
		result[1][1] = 1.0 / (tanHalfFov);
		result[2][3] = -1.0;

		result[2][2] = -(far + near) / (far - near);
		result[3][2] = -(2.0 * far * near) / (far - near);

		return result;
	}

	mat4 orthographic(double left, double right, double bottom, double top)
	{
		mat4 result;

		result[0][0] = 2.0 / (right - left);
		result[1][1] = 2.0 / (top - bottom);
		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);

		result[2][2] = -1;

		return result;
	}

	mat4 orthographic(double left, double right, double bottom, double top, double near, double far)
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
	}





	fvec2 tof(const vec2& v)	{ return fvec2(v.x, v.y); }
	fvec3 tof(const vec3& v)	{ return fvec3(v.x, v.y, v.z); }
	fvec4 tof(const vec4& v)	{ return fvec4(v.x, v.y, v.z, v.w); }

	vec2 fromf(const fvec2& v)	{ return vec2(v.x, v.y); }
	vec3 fromf(const fvec3& v)	{ return vec3(v.x, v.y, v.z); }
	vec4 fromf(const fvec4& v)	{ return vec4(v.x, v.y, v.z, v.w); }

	fmat2 tof(const mat2& m)	{ return fmat2(tof(m.vecs[0]), tof(m.vecs[1])); }
	fmat3 tof(const mat3& m)	{ return fmat3(tof(m.vecs[0]), tof(m.vecs[1]), tof(m.vecs[2])); }
	fmat4 tof(const mat4& m)	{ return fmat4(tof(m.vecs[0]), tof(m.vecs[1]), tof(m.vecs[2]), tof(m.vecs[3])); }

	mat2 fromf(const fmat2& m)	{ return mat2(fromf(m.vecs[0]), fromf(m.vecs[1])); }
	mat3 fromf(const fmat3& m)	{ return mat3(fromf(m.vecs[0]), fromf(m.vecs[1]), fromf(m.vecs[2])); }
	mat4 fromf(const fmat4& m)	{ return mat4(fromf(m.vecs[0]), fromf(m.vecs[1]), fromf(m.vecs[2]), fromf(m.vecs[3])); }


	std::vector<fvec2> tof(const std::vector<vec2>& vs) { return util::map(vs, [](const auto& v) -> auto { return tof(v); }); }
	std::vector<fvec3> tof(const std::vector<vec3>& vs) { return util::map(vs, [](const auto& v) -> auto { return tof(v); }); }
	std::vector<fvec4> tof(const std::vector<vec4>& vs) { return util::map(vs, [](const auto& v) -> auto { return tof(v); }); }

	std::vector<vec2> fromf(const std::vector<fvec2>& vs) { return util::map(vs, [](const auto& v) -> auto { return fromf(v); }); }
	std::vector<vec3> fromf(const std::vector<fvec3>& vs) { return util::map(vs, [](const auto& v) -> auto { return fromf(v); }); }
	std::vector<vec4> fromf(const std::vector<fvec4>& vs) { return util::map(vs, [](const auto& v) -> auto { return fromf(v); }); }
}


#include <iostream>
#include <iomanip>
namespace tinyformat
{
	static constexpr double THRESH = 1e-7;

	template <typename T>
	static T fix(T x)
	{
		if(lx::abs(x) < THRESH)
			return 0;

		return x;
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec2& v)
	{
		out << std::setprecision(3) << "v2(" << fix(v.x) << ", " << fix(v.y) << ")";
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec3& v)
	{
		out << std::setprecision(3) << "v3(" << fix(v.x) << ", " << fix(v.y) << ", " << fix(v.z) << ")";
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::vec4& v)
	{
		out << std::setprecision(3) << "v4(" << fix(v.x) << ", " << fix(v.y) << ", " << fix(v.z) << ", " << fix(v.w) << ")";
	}


	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec2& v)
	{
		out << std::setprecision(3) << "v2(" << fix(v.x) << ", " << fix(v.y) << ")";
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec3& v)
	{
		out << std::setprecision(3) << "v3(" << fix(v.x) << ", " << fix(v.y) << ", " << fix(v.z) << ")";
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::fvec4& v)
	{
		out << std::setprecision(3) << "v4(" << fix(v.x) << ", " << fix(v.y) << ", " << fix(v.z) << ", " << fix(v.w) << ")";
	}

	void formatValue(std::ostream& out, const char* /*fmtBegin*/, const char* fmtEnd, int ntrunc, const lx::quat& q)
	{
		out << std::setprecision(3) << "quat(" << fix(q.w) << ", " << fix(q.x) << ", " << fix(q.y) << ", " << fix(q.z) << ")";
	}
}













