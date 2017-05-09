// modules.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace Rx { struct Renderer; }

namespace Sotv
{
	struct GameState;
	struct Module
	{
		virtual void Render(GameState& gs, float delta, Rx::Renderer* ren);
		virtual void Update(GameState& gs, float delta);

		virtual ~Module() { }
	};

	struct PowerGenModule : Module
	{
		virtual size_t getProductionInWatts() = 0;

		protected:
		PowerGenModule();
	};

	struct SolarGenModule : PowerGenModule
	{
		size_t productionInWatts = 0;


		SolarGenModule(size_t watts);
		virtual size_t getProductionInWatts() override;
	};







	struct PowerStorageModule : Module
	{
		size_t current;
		size_t capacity;

		virtual size_t getEnergyInJoules();
		virtual size_t getCapacityInJoules();

		// returns the actual amount stored (because capacity was reached)
		virtual size_t storeEnergy(size_t joules);

		// returns the actual amount drained (because there wasn't enough stored)
		virtual size_t drainEnergy(size_t joules);

		protected:
		PowerStorageModule();
	};

	struct LithiumBatteryModule : PowerStorageModule
	{
		LithiumBatteryModule(size_t initialStorage, size_t cap);
	};
}













