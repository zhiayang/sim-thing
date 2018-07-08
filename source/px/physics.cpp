// simulator.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

#include "lx.h"
#include "lx/quat.h"

namespace px
{
	static constexpr double MINIMUM_VELOCITY		= 0.001;
	static constexpr double GRAVITATIONAL_CONSTANT	= 6.67408e-11;

	void stepSimulation(World& world, double dt)
	{
		world.worldtime += dt;
		for(auto& body : world.bodies)
		{
			body._netforce = integrators::Symplectic4(body, world, dt);
			body._inforce = lx::vec3(0);

			// for the next frame, apply a slight damping force.
			body.addForce(-body.velocity() * 5);


			if(lx::abs(body._vel.x) < MINIMUM_VELOCITY)	body._vel.x = 0;
			if(lx::abs(body._vel.y) < MINIMUM_VELOCITY)	body._vel.y = 0;
			if(lx::abs(body._vel.z) < MINIMUM_VELOCITY)	body._vel.z = 0;
		}
	}

	lx::vec3 getForce(const RigidBody& self, const World& world, double dt)
	{
		lx::vec3 net = self._inforce;

		for(const auto& body : world.bodies)
		{
			if(body == self) continue;

			// F = GMm/r2. split it up into (GM/r) * (m/r)
			double dist = lx::distance(self.position(), body.position());
			double mag = ((GRAVITATIONAL_CONSTANT * self.mass) / dist) * (body.mass / dist);
			auto dir = lx::vec3(body.position() - self.position()).normalised();

			net += dir * mag;
		}

		return net;
	}
}















