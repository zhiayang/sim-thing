// simulator.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

#include "lx.h"
#include "lx/quat.h"

namespace px
{
	static constexpr double MINIMUM_VELOCITY        = 0.00003;
	static constexpr double MINIMUM_ROT_VELOCITY    = 0.00003;
	static constexpr double GRAVITATIONAL_CONSTANT  = 6.67408e-11;

	// simulate water
	static constexpr double FLUID_DENSITY           = 1000.0;
	static constexpr double DAMPING_TORQUE_FACTOR   = 0.9;

	static void applyDampingForces(RigidBody& body)
	{
		// for the next frame, apply a slight damping force
		if(body.velocity().magnitudeSquared() > MINIMUM_VELOCITY * MINIMUM_VELOCITY)
		{
			double forceMag = -0.5 * FLUID_DENSITY * body.velocity().magnitudeSquared()
				* body.dragCoefficient * body.surfaceArea;

			body.addForce(body.velocity().normalised() * forceMag);
		}

		// for the next frame, apply a slight damping torque
		if(body.angularVelocity().magnitudeSquared() > MINIMUM_ROT_VELOCITY * MINIMUM_ROT_VELOCITY)
			body.addTorque(body.angularMomentum() * -DAMPING_TORQUE_FACTOR);
	}

	static void clampVelocities(RigidBody& body)
	{
		// clamp the velocity.
		if(lx::abs(body._vel.x) < MINIMUM_VELOCITY) body._linearMtm.x = 0;
		if(lx::abs(body._vel.y) < MINIMUM_VELOCITY) body._linearMtm.y = 0;
		if(lx::abs(body._vel.z) < MINIMUM_VELOCITY) body._linearMtm.z = 0;

		// clamp the angular velocity as well
		if(lx::abs(body._angVel.x) < MINIMUM_ROT_VELOCITY)  body._angularMtm.x = 0, body._angVel.x = 0;
		if(lx::abs(body._angVel.y) < MINIMUM_ROT_VELOCITY)  body._angularMtm.y = 0, body._angVel.y = 0;
		if(lx::abs(body._angVel.z) < MINIMUM_ROT_VELOCITY)  body._angularMtm.z = 0, body._angVel.z = 0;
	}

	void stepSimulation(World& world, double dt)
	{
		world.worldtime += dt;
		for(auto& body : world.bodies)
		{
			applyDampingForces(body);

			if(!body.immovable)
				integrators::Symplectic4(body, world, dt);

			clampVelocities(body);

			// zero out the net force for the next step.
			body._force = lx::vec3(0);
			body._torque = lx::vec3(0);

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















