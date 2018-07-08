// px.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <vector>
#include <stdint.h>

#include "lx.h"

#include "px/inertia.h"

namespace px
{
	struct RigidBody
	{
		size_t id = 0;

		// intrinsic state of the body
		double mass;
		lx::mat3 _bodyInertiaMoment;

		lx::vec3 _pos;
		lx::quat _rot;
		lx::vec3 _linearMtm;
		lx::vec3 _angularMtm;

		// calculated by the integrator
		lx::vec3 _vel;

		// input forces
		lx::vec3 _force;
		lx::vec3 _torque;


		RigidBody(double m, const lx::vec3& p, const lx::vec3& v, const lx::quat& r, const lx::mat3& inertiaMoment);

		const lx::vec3& position() const { return this->_pos; }
		const lx::vec3& velocity() const { return this->_vel; }
		const lx::quat& rotation() const { return this->_rot; }
		const lx::vec3& linearMomentum() const { return this->_linearMtm; }
		const lx::vec3& angularMomentum() const { return this->_angularMtm; }

		void addForceAt(const lx::vec3& pos, const lx::vec3& force);

		const lx::vec3& getForce() const { return this->_force; }
		const lx::vec3& getTorque() const { return this->_torque; }

		bool operator == (const RigidBody& b) const { return this->id == b.id; }
	};

	struct World
	{
		double worldtime = 0;
		std::vector<RigidBody> bodies;
	};

	void stepSimulation(World& world, double dt);
	lx::vec3 getForce(const RigidBody& body, const World& world, double dt);







	namespace integrators
	{
		void Symplectic4(RigidBody& body, const World& world, double dt);
		void Symplectic3(RigidBody& body, const World& world, double dt);
		void Verlet2(RigidBody& body, const World& world, double dt);
		void Euler1(RigidBody& body, const World& world, double dt);
	}

}













