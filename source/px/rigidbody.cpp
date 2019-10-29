// rigidbody.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"
#include "rx.h"
#include "tinyformat.h"

namespace px
{
	static size_t __id = 0;
	RigidBody::RigidBody(double m, const lx::vec3& p, const lx::vec3& v, const lx::quat& r, const lx::mat3& bodyTensor,
		const rx::Mesh& cmesh) : collisionMesh(cmesh)
	{
		this->id = __id++;
		this->immovable = false;

		// constants
		this->mass = m;
		this->_bodyInertiaTensor = bodyTensor;
		this->_inverseBodyInertiaTensor = this->_bodyInertiaTensor.inversed();

		// initial settings
		this->_pos = p;
		this->_rot = r;
		this->_linearMtm = this->mass * v;
	}

	void RigidBody::addForce(const lx::vec3& force)
	{
		this->_force += force;
		// this->_torque += lx::cross(this->position(), force);
	}

	void RigidBody::addTorque(const lx::vec3& torque)
	{
		this->_torque += this->rotation().inversed() * torque;
	}


	void RigidBody::addRelForceAt(const lx::vec3& pos, const lx::vec3& force)
	{
		this->_force += this->rotation() * force;
		this->_torque += this->rotation() * lx::cross(pos, force);
	}

	void RigidBody::addRelTorque(const lx::vec3& torque)
	{
		this->_torque += torque;
	}

	void RigidBody::addAngularVelocity(const lx::vec3& w)
	{
		this->_inputOmega += w;
	}
}















