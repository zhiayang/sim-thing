// integrators.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"

namespace px {
namespace integrators
{
	lx::vec3 Symplectic4(RigidBody& body, const World& world, double dt)
	{
		// constants for 4th order integration.
		// copied from https://en.wikipedia.org/wiki/Symplectic_integrator
		static constexpr double c[4] = {
			+0.67560359597982881702384390448573041346,
			-0.17560359597982881702384390448573041346,
			-0.17560359597982881702384390448573041346,
			+0.67560359597982881702384390448573041346
		};

		static constexpr double d[4] = {
			+1.35120719195965763404768780897146082692,
			-1.70241438391931526809537561794292165384,
			+1.35120719195965763404768780897146082692,
			+0.00000000000000000000000000000000000000
		};

		auto v0 = body._vel;
		for(int i = 0; i < 4; i++)
		{
			body._vel += (getForce(body, world, dt) / body.mass) * c[i] * dt;
			body._pos += body._vel * d[i] * dt;
		}

		// F = dp/dt
		return (body.mass * (body._vel - v0)) / dt;
	}

	lx::vec3 Symplectic3(RigidBody& body, const World& world, double dt)
	{
		// constants for 3rd order integration.
		// copied from https://en.wikipedia.org/wiki/Symplectic_integrator
		static constexpr double c[3] = {
			+1,
			-0.66666666666666666666666666666666666666,
			+0.66666666666666666666666666666666666666
		};

		static constexpr double d[3] = {
			-0.04166666666666666666666666666666666666,
			+0.75000000000000000000000000000000000000,
			+0.29166666666666666666666666666666666666
		};

		auto v0 = body._vel;
		for(int i = 0; i < 3; i++)
		{
			body._vel += (getForce(body, world, dt) / body.mass) * c[i] * dt;
			body._pos += body._vel * d[i] * dt;
		}

		return (body.mass * (body._vel - v0)) / dt;
	}


	lx::vec3 Verlet2(RigidBody& body, const World& world, double dt)
	{
		auto v0 = body._vel;

		body._pos += body._vel * 0.5 * dt;
		body._vel += (getForce(body, world, dt) / body.mass) * dt;
		body._pos += body._vel * 0.5 * dt;

		return (body.mass * (body._vel - v0)) / dt;
	}


	lx::vec3 Euler1(RigidBody& body, const World& world, double dt)
	{
		auto v0 = body._vel;

		body._vel += (getForce(body, world, dt) / body.mass) * dt;
		body._pos += body._vel * dt;

		return (body.mass * (body._vel - v0)) / dt;
	}
}
}




