// integrator.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"
#include "lx/quat.h"

namespace px
{
	// gravitational constant.
	static constexpr double GRAVITATIONAL_CONSTANT = 6.67408e-11;
	// static constexpr double GRAVITATIONAL_CONSTANT = 5;



	void integrateSymplectic4(RigidBody& body, const World& world, double dt)
	{
		// constants for 4th order integration.
		// copied from https://en.wikipedia.org/wiki/Symplectic_integrator
		static constexpr double c1 = +0.67560359597982881702384390448573041346;
		static constexpr double c2 = -0.17560359597982881702384390448573041346;
		static constexpr double c3 = -0.17560359597982881702384390448573041346;
		static constexpr double c4 = +0.67560359597982881702384390448573041346;
		static constexpr double d1 = +1.35120719195965763404768780897146082692;
		static constexpr double d2 = -1.70241438391931526809537561794292165384;
		static constexpr double d3 = +1.35120719195965763404768780897146082692;
		static constexpr double d4 = +0.00000000000000000000000000000000000000;

		body._vel += (getForce(body, world, dt) / body.mass) * c1 * dt;
		body._pos += body._vel * d1 * dt;

		body._vel += (getForce(body, world, dt) / body.mass) * c2 * dt;
		body._pos += body._vel * d2 * dt;

		body._vel += (getForce(body, world, dt) / body.mass) * c3 * dt;
		body._pos += body._vel * d3 * dt;

		body._vel += (getForce(body, world, dt) / body.mass) * c4 * dt;
		body._pos += body._vel * d4 * dt;
	}


	void integrateVerlet2(RigidBody& body, const World& world, double dt)
	{
		static constexpr double c1 = 0;
		static constexpr double c2 = 1;
		static constexpr double d1 = 0.5;
		static constexpr double d2 = 0.5;

		body._vel += (getForce(body, world, dt) / body.mass) * c1 * dt;
		body._pos += body._vel * d1 * dt;

		body._vel += (getForce(body, world, dt) / body.mass) * c2 * dt;
		body._pos += body._vel * d2 * dt;
	}


	void integrateEuler1(RigidBody& body, const World& world, double dt)
	{
		body._vel += (getForce(body, world, dt) / body.mass) * dt;
		body._pos += body._vel * dt;
	}





	void simulateWorld(World& world, double dt)
	{
		for(auto& body : world.bodies)
		{
			integrateSymplectic4(body, world, dt);
			// fprintf(stderr, "body (%zd): pos(%+.1f, %+.1f, %+.1f), vel(%+.1f, %+.1f, %+.1f)\n",
			// 	body.id, body._pos.x, body._pos.y, body._pos.z, body._vel.x, body._vel.y, body._vel.z);
		}
	}

	lx::vec3 getForce(const RigidBody& self, const World& world, double dt)
	{
		lx::vec3 net;

		for(const auto& body : world.bodies)
		{
			if(body == self) continue;

			// F = GMm/r2. split it up into (GM/r) * (m/r)
			double dist = lx::distance(self.position(), body.position());
			double mag = ((GRAVITATIONAL_CONSTANT * self.mass) / dist) * (body.mass / dist);
			auto dir = lx::vec3(body.position() - self.position()).normalised();

			net += dir * mag;
			// fprintf(stderr, "id: %zd, (dist = %.1f, mag = %.1f, dir = (%.1f, %.1f, %.1f)), force = (%.1f, %.1f, %.1f)\n", self.id,
			// 	dist, mag, dir.x, dir.y, dir.z, net.x, net.y, net.z);
		}

		return net;
	}
}















