// inertia.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"

namespace px
{
	lx::mat3 getInertiaMomentOfCylinder(double radius, double height);
	lx::mat3 getInertiaMomentOfCuboid(const lx::vec3& dims);
	lx::mat3 getInertiaMomentOfSphere(double radius);
}
