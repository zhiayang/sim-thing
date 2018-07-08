// rigidbody.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

namespace px
{
	static size_t __id = 0;
	RigidBody::RigidBody(double m, const lx::vec3& p, const lx::vec3& v)
	{
		this->id = __id++;

		// mass.
		this->mass = m;

		// initial position and velocity
		this->_pos = p;
		this->_vel = v;
	}

	void RigidBody::addForce(const lx::vec3& f)
	{
		this->_inforce += f;
	}
}
