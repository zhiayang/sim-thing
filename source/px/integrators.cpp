// integrators.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"
#include "tinyformat.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace px {
namespace integrators
{
	static void _doSymplectic(const double* c, const double* d, int steps, RigidBody& body, const World& world, double dt)
	{
		for(int i = 0; i < steps; i++)
		{
			auto [ force, torque ] = calculateForceAndTorque(body, world, dt);

			body._pos += body._vel * c[i] * dt;
			body._linearMtm += force * d[i] * dt;
			body._vel = body._linearMtm / body.mass;

			// transform the body-space inertia tensor matrix into a world-space one
			auto rotmat = body._rot.normalised().toRotationMatrix();

			auto iibody = (body._bodyInertiaTensor * body.mass).inversed();
			auto tensor = rotmat * iibody * rotmat.transposed();

			body._rot += body._rot * lx::quat(0, body._angVel) * 0.5 * c[i] * dt;
			body._rot = body._rot.normalised();

			body._angularMtm += torque * d[i] * dt;
			body._angVel = body._inputOmega + (tensor * body._angularMtm);

			// tfm::printfln("τ = %s, L = %s, ω = %s", torque, body._angularMtm, body._angVel);
		}
	}



	void Symplectic4(RigidBody& body, const World& world, double dt)
	{
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

		_doSymplectic(c, d, 4, body, world, dt);
	}

	void Symplectic3(RigidBody& body, const World& world, double dt)
	{
		static constexpr double c[3] = {
			+1.00000000000000000000000000000000000000,
			-0.66666666666666666666666666666666666666,
			+0.66666666666666666666666666666666666666
		};

		static constexpr double d[3] = {
			-0.04166666666666666666666666666666666666,
			+0.75000000000000000000000000000000000000,
			+0.29166666666666666666666666666666666666
		};

		_doSymplectic(c, d, 3, body, world, dt);
	}
}
}




