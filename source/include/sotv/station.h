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


		void addSystem(System* sys);

		void Render(GameState& s, float delta, Rx::Renderer* ren);
		void Update(GameState& s, float delta);

		static Station* makeDefaultSpaceStation(std::string name);

		private:
		Station() { }
	};
}













