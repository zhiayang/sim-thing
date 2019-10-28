// px.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <algorithm>
#include <vector>
#include <stdint.h>

#include "lx.h"

#include "px/inertia.h"

namespace rx
{
	struct Mesh;
}

namespace px
{
	struct RigidBody
	{
		size_t id = 0;

		// intrinsic properties of the body
		double mass;
		bool immovable;
		lx::mat3 _bodyInertiaTensor;
		lx::mat3 _inverseBodyInertiaTensor;

		// state
		lx::vec3 _pos;
		lx::quat _rot;

		lx::vec3 _linearMtm;
		lx::vec3 _angularMtm;

		// calculated by the integrator
		lx::vec3 _vel;
		lx::vec3 _angVel;

		// input forces
		lx::vec3 _force;
		lx::vec3 _torque;
		lx::vec3 _inputOmega;


		// collision stuff
		const rx::Mesh& collisionMesh;


		RigidBody(double m, const lx::vec3& p, const lx::vec3& v, const lx::quat& r, const lx::mat3& inertiaTensor,
			const rx::Mesh& cmesh);

		const lx::vec3& position() const { return this->_pos; }
		const lx::vec3& velocity() const { return this->_vel; }
		const lx::vec3& linearMomentum() const { return this->_linearMtm; }

		const lx::quat& rotation() const { return this->_rot; }
		const lx::vec3& angularVelocity() const { return this->_angVel; }
		const lx::vec3& angularMomentum() const { return this->_angularMtm; }


		void addRelForceAt(const lx::vec3& pos, const lx::vec3& force);
		void addRelTorque(const lx::vec3& torque);

		void addForceAt(const lx::vec3& pos, const lx::vec3& force);
		void addTorque(const lx::vec3& torque);

		void addAngularVelocity(const lx::vec3& w);

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
	std::pair<lx::vec3, lx::vec3> calculateForceAndTorque(const RigidBody& body, const World& world, double dt);





	namespace collision
	{
		struct Octree
		{
			struct Node
			{
				lx::vec3 minCorner;
				lx::vec3 maxCorner;

				std::vector<RigidBody*> bodies;

				Node* parent;
				Node* children[8];
			};

			lx::vec3 lowerBound;
			lx::vec3 upperBound;

			Node* root = 0;



		};
	}

	namespace integrators
	{
		void Symplectic4(RigidBody& body, const World& world, double dt);
		void Symplectic3(RigidBody& body, const World& world, double dt);
		void Verlet2(RigidBody& body, const World& world, double dt);
		void Euler1(RigidBody& body, const World& world, double dt);
	}

}













