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
	struct PowerSystem;

	struct Module
	{
		virtual bool isActivated() { return this->activated; }
		virtual void deactivate() { this->activated = false; }
		virtual void activate() { this->activated = true; }
		virtual bool toggle() { return (this->activated = !this->activated); }


		virtual void Render(GameState& gs, double delta, Rx::Renderer* ren);
		virtual void Update(GameState& gs, double delta);

		virtual ~Module() { }

		protected:
		bool activated = false;
	};

	struct PowerGenModule : Module
	{
		virtual double getProductionInWatts() = 0;

		protected:
		PowerGenModule();
	};

	struct SolarGenModule : PowerGenModule
	{
		SolarGenModule(double watts);
		virtual double getProductionInWatts() override;

		protected:
		double productionInWatts = 0;
	};







	struct PowerStorageModule : Module
	{
		virtual double getEnergyInJoules();
		virtual double getCapacityInJoules();

		// returns the actual amount stored (because capacity was reached)
		virtual double storeEnergy(double joules);

		// returns the actual amount drained (because there wasn't enough stored)
		virtual double drainEnergy(double joules);

		protected:

		double current;
		double capacity;

		PowerStorageModule();
	};

	struct LithiumBatteryModule : PowerStorageModule
	{
		LithiumBatteryModule(double initialStorage, double cap);
	};





	struct PowerConsumerModule : Module
	{
		PowerConsumerModule(double consumingCurrent);
		virtual double getCurrentInAmps();
		virtual double getMaximumCurrentInAmps();

		virtual void setMaximumCurrentInAmps(double current);

		virtual double rampCurrentTo(double c);
		virtual double resetCurrent();

		protected:
		double current;
		double maximumCurrent;
	};


}













