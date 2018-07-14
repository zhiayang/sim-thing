// integrators.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "px.h"
#include "tinyformat.h"

namespace px {
namespace integrators
{
	static void _doSymplectic(const double* c, const double* d, int steps, RigidBody& body, const World& world, double dt)
	{
		for(int i = 0; i < steps; i++)
		{
			lx::vec3 force, torque;
			std::tie(force, torque) = calculateForceAndTorque(body, world, dt);

			body._linearMtm += force * c[i] * dt;
			body._vel = body._linearMtm / body.mass;
			body._pos += body._vel * d[i] * dt;


			// transform the body-space inertia tensor matrix into a world-space one
			// auto rotmat = body.rotation().toRotationMatrix();
			auto rotmat = lx::mat3();
			auto invtensor = rotmat * (body._bodyInertiaTensor * body.mass).inversed() * rotmat.transposed();

			body._angularMtm += torque * c[i] * dt;
			body._angVel = body._inputOmega + (invtensor * body._angularMtm);

			// body._rot = lx::quat::fromRotationMatrix((body._angVel * body._rot.toRotationMatrix()) * d[i] * dt);
			lx::quat qdot = 0.5 * body._rot * lx::quat(0, body._angVel) * d[i] * dt;
			body._rot += qdot;
			body._rot = body._rot.normalised();

			// tfm::printf("rot = %s\n", qdot.toEulerDegs());
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


	void Verlet2(RigidBody& body, const World& world, double dt)
	{
		body._pos += body._vel * 0.5 * dt;
		body._linearMtm += calculateForceAndTorque(body, world, dt).first * dt;
		body._pos += body._vel * 0.5 * dt;
	}


	void Euler1(RigidBody& body, const World& world, double dt)
	{
		body._linearMtm += calculateForceAndTorque(body, world, dt).first * dt;
		body._pos += body._vel * dt;
	}
}
}




