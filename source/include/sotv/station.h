// station.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <vector>

#include "sotv/modules.h"
#include "sotv/systems.h"

namespace Rx { struct Renderer; }

namespace Sotv
{
	struct GameState;


	struct Station
	{
		std::string stationName;
		std::vector<System*> systems;


		// 'special' systems, in a sense; they're still in the list above, it's just here to be easier to reference
		PowerSystem* powerSystem = 0;
		LifeSupportSystem* lifeSupportSystem = 0;

		void addSystem(System* sys);


		double getInternalVolume() { return this->internalVolume; }
		void setInternalVolume(double newVolume) { this->internalVolume = newVolume; }

		// use delta < 0 to decrease.
		double increaseInternalVolume(double delta) { return (this->internalVolume += delta); }

		void Render(GameState& s, double delta, Rx::Renderer* ren);
		void Update(GameState& s, double delta);

		static Station* makeDefaultSpaceStation(std::string name);

		private:
		Station() { }

		double internalVolume = 0;
	};
}













