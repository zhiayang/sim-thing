// systems.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <vector>

#include "sotv/modules.h"

namespace rx { struct Renderer; }


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

		virtual void Render(GameState& gs, double delta, rx::Renderer* ren);
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

		std::vector<PowerGenModule*> generators;
		std::vector<PowerStorageModule*> storage;
		std::vector<PowerConsumerModule*> consumers;

		void addGenerator(PowerGenModule* gen);
		void addStorage(PowerStorageModule* stor);
		void addConsumer(PowerConsumerModule* cons);

		double consumeEnergy(double watts, double delta);

		double getTotalConsumptionInWatts();
		double getTotalProductionInWatts();

		double getTotalStorageInJoules();
		double getTotalCapacityInJoules();

		virtual void Render(GameState& gs, double delta, rx::Renderer* ren) override;
		virtual void Update(GameState& gs, double delta) override;

		protected:
		double previousExcess = 0;
		double totalConsumed = 0;
	};




	struct LifeSupportSystem : System
	{
		struct AtmosphereGenerator
		{
			AtmosphereGenerator(double joules, double amps)
			{
				this->joulesPerGramOfOxygen = joules;
				this->powerConsumer = new PowerConsumerModule(amps);
			}

			void updateAtmosphere(Station* stn, LifeSupportSystem* lss, double delta);

			PowerConsumerModule* powerConsumer;
			double joulesPerGramOfOxygen;
		};

		struct ThermalControl
		{
			ThermalControl(double heateramps, double cooleramps, double coolerEfficiency)
			{
				this->maxCoolingAmps = cooleramps;
				this->maxHeatingAmps = heateramps;
				this->coolingEfficiency = coolerEfficiency;

				this->heatingConsumer = new PowerConsumerModule(this->maxHeatingAmps);
				this->coolingConsumer = new PowerConsumerModule(this->maxCoolingAmps);
			}

			void updateThermalControl(Station* stn, LifeSupportSystem* lss, double delta);

			PowerConsumerModule* heatingConsumer;
			PowerConsumerModule* coolingConsumer;

			double maxCoolingAmps;
			double maxHeatingAmps;
			double coolingEfficiency;
		};

		friend struct AtmosphereGenerator;
		friend struct ThermalControl;

		LifeSupportSystem(Station* stn, double targetPressure, double targetTemperature);

		virtual void Render(GameState& gs, double delta, rx::Renderer* ren) override;
		virtual void Update(GameState& gs, double delta) override;

		virtual bool toggle() override;

		double getAtmosphereTemperature();
		double getAtmospherePressure();

		void addWasteHeatToSystem(double watts, double delta);

		protected:
		double massOfOxygenInKilograms;
		double massOfNitrogenInKilograms;
		double internalTemperatureInKelvin;

		// separate consumers for each process the LSS is responsible for
		AtmosphereGenerator* atmosGenerator;
		ThermalControl* thermalController;

		PowerConsumerModule* waterConsumer;

		const double targetPressureInPascals;
		const double targetTemperatureInKelvin;

		void generateWater(GameState& gs, double delta);
	};
}



























