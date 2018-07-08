// rigidbody.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

namespace px
{
	static size_t __id = 0;
	RigidBody::RigidBody(double m, const lx::vec3& p, const lx::vec3& v, const lx::quat& r, const lx::mat3& bodyInertia)
	{
		this->id = __id++;

		// constants
		this->mass = m;
		this->_bodyInertiaMoment = bodyInertia;

		// initial settings
		this->_pos = p;
		this->_rot = r;
		this->_linearMtm = this->mass * v;
	}

	void RigidBody::addForceAt(const lx::vec3& pos, const lx::vec3& force)
	{
		this->_force += force;
	}
}
