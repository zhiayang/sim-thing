// simulator.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

#include "lx.h"
#include "lx/quat.h"

namespace px
{
	static constexpr double MINIMUM_VELOCITY		= 0.005;
	static constexpr double GRAVITATIONAL_CONSTANT	= 6.67408e-11;

	void stepSimulation(World& world, double dt)
	{
		world.worldtime += dt;
		for(auto& body : world.bodies)
		{
			integrators::Symplectic4(body, world, dt);

			// zero out the net force for the next step.
			body._force = lx::vec3(0);
			body._torque = lx::vec3(0);

			// clamp the velocity.
			if(lx::abs(body._vel.x) < MINIMUM_VELOCITY)	body._vel.x = 0;
			if(lx::abs(body._vel.y) < MINIMUM_VELOCITY)	body._vel.y = 0;
			if(lx::abs(body._vel.z) < MINIMUM_VELOCITY)	body._vel.z = 0;

			// clamp the angular velocity as well
			if(lx::abs(body._linearMtm.x) < MINIMUM_VELOCITY)	body._linearMtm.x = 0;
			if(lx::abs(body._linearMtm.y) < MINIMUM_VELOCITY)	body._linearMtm.y = 0;
			if(lx::abs(body._linearMtm.z) < MINIMUM_VELOCITY)	body._linearMtm.z = 0;


			// for the next frame, apply a slight damping force
			if(body.velocity().magnitudeSquared() > MINIMUM_VELOCITY * MINIMUM_VELOCITY)
				body.addRelForceAt(lx::vec3(), -1 * (body.velocity() * body.mass * 0.5));

			// for the next frame, apply a slight damping torque
			if(body.angularMomentum().magnitudeSquared() > MINIMUM_VELOCITY * MINIMUM_VELOCITY)
				body.addTorque(body.angularMomentum() * -0.3);
		}
	}

	std::pair<lx::vec3, lx::vec3> calculateForceAndTorque(const RigidBody& self, const World& world, double dt)
	{
		lx::vec3 force = self._force;
		lx::vec3 torque = self._torque;

		for(const auto& body : world.bodies)
		{
			if(body == self) continue;

			// F = GMm/r2. split it up into (GM/r) * (m/r)
			double dist = lx::distance(self.position(), body.position());
			double mag = ((GRAVITATIONAL_CONSTANT * self.mass) / dist) * (body.mass / dist);
			auto dir = lx::vec3(body.position() - self.position()).normalised();

			force += dir * mag;
		}

		return { force, torque };
	}
}















