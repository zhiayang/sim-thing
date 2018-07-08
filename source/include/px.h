// px.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <lx.h>
#include <vector>
#include <stdint.h>

namespace px
{
	struct RigidBody
	{
		size_t id = 0;

		double mass;

		lx::vec3 _pos;
		lx::vec3 _vel;

		lx::vec3 _inforce;
		lx::vec3 _netforce;

		lx::mat3 _rotation;


		RigidBody(double m, const lx::vec3& p, const lx::vec3& v);

		const lx::vec3& position() const { return this->_pos; }
		const lx::vec3& velocity() const { return this->_vel; }

		void addForce(const lx::vec3& f);

		const lx::vec3& getNetForce() const { return this->_netforce; }
		const lx::vec3& getInputForce() const { return this->_inforce; }

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
		// returns the net force on the object, at the end of the simulation step.
		lx::vec3 Symplectic4(RigidBody& body, const World& world, double dt);
		lx::vec3 Symplectic3(RigidBody& body, const World& world, double dt);
		lx::vec3 Verlet2(RigidBody& body, const World& world, double dt);
		lx::vec3 Euler1(RigidBody& body, const World& world, double dt);
	}

}













