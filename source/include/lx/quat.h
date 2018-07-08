// quat.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

#include "lx/vec4.h"
#include "lx/mat3.h"

namespace lx
{
	struct quat
	{
		explicit quat(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) { }
		explicit quat(double r, const vec3& i) : real(r), imag(i) { }
		explicit quat() : real(1.0), imag(0.0) { }

		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wzero-length-array"
		union
		{
			double ptr[0];
			struct { double w; double x; double y; double z; };
			struct { double real; vec3 imag; };
		};
		#pragma clang diagnostic pop

		double& operator[] (size_t i);
		const double& operator[] (size_t i) const;
		quat& operator += (const quat& v);
		quat& operator -= (const quat& v);
		quat& operator *= (const quat& v);

		quat& operator *= (double s);
		quat& operator /= (double s);

		vec3 toEulerDegs() const;
		vec3 toEulerRads() const;
		mat3 toRotationMatrix() const;

		quat normalised() const;
		double magnitude() const;

		static quat fromEulerDegs(const vec3& elr);
		static quat fromEulerRads(const vec3& elr);
		static quat fromRotationMatrix(const mat3& r);
	};

	quat normalise(const quat& q);
	double magnitude(const quat& v);

	quat operator + (const quat& a, const quat& b);
	quat operator - (const quat& a, const quat& b);
	quat operator * (const quat& a, const quat& b);
	bool operator == (const quat& a, const quat& b);

	quat operator * (const quat& a, double b);
	quat operator / (const quat& a, double b);
	quat operator * (double a, const quat& b);
}






