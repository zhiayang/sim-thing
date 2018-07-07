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

		RigidBody(double m, const lx::vec3& p, const lx::vec3& v);

		const lx::vec3& position() const { return this->_pos; }
		const lx::vec3& velocity() const { return this->_vel; }


		bool operator == (const RigidBody& b) const { return this->id == b.id; }
	};

	struct World
	{
		std::vector<RigidBody> bodies;
	};

	void simulateWorld(World& world, double dt);
	lx::vec3 getForce(const RigidBody& body, const World& world, double dt);
}
