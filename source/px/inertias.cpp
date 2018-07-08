// inertias.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

namespace px
{
	// taken from https://gist.github.com/awesomebytes/39a4ba6c64956a1aa9bd
	// which was taken from wolfram alpha.

	lx::mat3 getInertiaMomentOfCylinder(double radius, double height)
	{
		auto rad2 = radius * radius;
		auto hgt2 = height * height;

		double ixx = (1.0 / 12.0) * (3.0 * rad2 + hgt2);
		double iyy = (1.0 / 12.0) * (3.0 * rad2 + hgt2);
		double izz = (1.0 / 12.0) * rad2;

		return lx::mat3(
			ixx, 0, 0,
			0, iyy, 0,
			0, 0, izz
		);
	}

	lx::mat3 getInertiaMomentOfCuboid(const lx::vec3& dims)
	{
		double x2 = dims.x * dims.x;
		double y2 = dims.y * dims.y;
		double z2 = dims.z * dims.z;

		double ixx = (1.0 / 12.0) * (y2 + z2);
		double iyy = (1.0 / 12.0) * (x2 + z2);
		double izz = (1.0 / 12.0) * (x2 + y2);

		return lx::mat3(
			ixx, 0, 0,
			0, iyy, 0,
			0, 0, izz
		);
	}

	lx::mat3 getInertiaMomentOfSphere(double radius)
	{
		double i = (2.0 / 5.0) * radius * radius;

		return lx::mat3(
			i, 0, 0,
			0, i, 0,
			0, 0, i
		);
	}
}








