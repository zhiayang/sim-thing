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
	struct Station;
	struct GameState;

	struct System
	{
		void addModule(Module* mod);
		std::vector<Module*>& getModules() { return this->modules; }

		virtual bool isActivated() { return this->activated; }
		virtual void deactivate();
		virtual void activate();
		virtual bool toggle();

		virtual void Render(GameState& gs, double delta, Rx::Renderer* ren);
		virtual void Update(GameState& gs, double delta);

		System(Station* stn) : station(stn) { }
		virtual ~System() { }

		protected:
		Station* station;
		bool activated = false;
		std::vector<Module*> modules;
	};



	struct PowerSystem : System
	{
		// note that these modules being pointed to are *not mutually exclusive* with the parent `modules` vector;
		// the parent handles the generic update/render of each module,
		// while we handle the intricacies of moving information between each module during the update.

		PowerSystem(Station* stn);

		const size_t systemVoltage;

		double previousExcess = 0;

		std::vector<PowerGenModule*> generators;
		std::vector<PowerStorageModule*> storage;
		std::vector<PowerConsumerModule*> consumers;

		void addGenerator(PowerGenModule* gen);
		void addStorage(PowerStorageModule* stor);
		void addConsumer(PowerConsumerModule* cons);

		double getTotalProductionInWatts();
		double getTotalStorageInJoules();
		double getTotalCapacityInJoules();

		virtual void Render(GameState& gs, double delta, Rx::Renderer* ren) override;
		virtual void Update(GameState& gs, double delta) override;
	};




	struct LifeSupportSystem : System
	{
		LifeSupportSystem(Station* stn, double targetPressure, double targetTemperature);

		virtual void Render(GameState& gs, double delta, Rx::Renderer* ren) override;
		virtual void Update(GameState& gs, double delta) override;

		virtual bool toggle() override;

		double getAtmosphereTemperature();
		double getAtmospherePressure();

		protected:
		double massOfOxygenInKilograms;
		double massOfNitrogenInKilograms;
		double internalTemperatureInKelvin;

		// oxgygen stuff
		double joulesPerGramOfOxygen;

		// separate consumers for each process the LSS is responsible for
		PowerConsumerModule* atmosphereConsumer;
		PowerConsumerModule* heatingConsumer;
		PowerConsumerModule* waterConsumer;

		const double targetPressureInPascals;
		const double targetTemperatureInKelvin;

		void generateOxygen(GameState& gs, double delta);
		void generateWater(GameState& gs, double delta);
		void generateHeat(GameState& gs, double delta);
	};
}



























