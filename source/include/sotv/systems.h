// systems.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <vector>

#include "sotv/modules.h"

namespace Rx { struct Renderer; }


namespace Sotv
{
	struct GameState;

	struct System
	{
		std::vector<Module*> modules;

		void addModule(Module* mod);

		virtual void Render(GameState& gs, float delta, Rx::Renderer* ren);
		virtual void Update(GameState& gs, float delta);


		virtual ~System() { }
	};



	struct PowerSystem : System
	{
		// note that these modules being pointed to are *not mutually exclusive* with the parent `modules` vector;
		// the parent handles the generic update/render of each module,
		// while we handle the intricacies of moving information between each module during the update.

		std::vector<PowerGenModule*> generators;
		std::vector<PowerStorageModule*> storage;

		void addGenerator(PowerGenModule* gen);
		void addStorage(PowerStorageModule* stor);

		size_t getTotalProductionInWatts();
		size_t getTotalStorageInJoules();
		size_t getTotalCapacityInJoules();

		virtual void Render(GameState& gs, float delta, Rx::Renderer* ren) override;
		virtual void Update(GameState& gs, float delta) override;
	};
}



























