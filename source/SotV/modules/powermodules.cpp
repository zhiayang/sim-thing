// powermodules.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"

namespace Sotv
{
	PowerGenModule::PowerGenModule() { }
	PowerStorageModule::PowerStorageModule() { }


	SolarGenModule::SolarGenModule(double area, double eff)
	{
		this->solarPanelArea = area;
		this->solarCellEfficiency = eff;
	}

	// this is the wattage per square metre near earth.
	#define TOTAL_AVAILABLE_SOLAR_POWER		1350
	double SolarGenModule::getProductionInWatts()
	{
		// assume we are at europa (jupiter) - 5.2^2 = 27 times weaker.
		double totalSolarPower = TOTAL_AVAILABLE_SOLAR_POWER / 27.0;

		return this->solarPanelArea * this->solarCellEfficiency * totalSolarPower;
	}





	LithiumBatteryModule::LithiumBatteryModule(double initialStorage, double cap)
	{
		this->current = initialStorage;
		this->capacity = cap;

		assert(this->current <= this->capacity);
	};

	double PowerStorageModule::getEnergyInJoules()
	{
		return this->current;
	}

	double PowerStorageModule::getCapacityInJoules()
	{
		return this->capacity;
	}

	double PowerStorageModule::storeEnergy(double joules)
	{
		double left = this->capacity - this->current;
		this->current = std::min(this->capacity, this->current + joules);

		return std::min(joules, left);
	}

	double PowerStorageModule::drainEnergy(double joules)
	{
		if(this->current >= joules)
		{
			this->current -= joules;
			return joules;
		}
		else
		{
			double ret = this->current;
			this->current = 0;

			return ret;
		}
	}




	PowerConsumerModule::PowerConsumerModule(double consumingCurrent)
	{
		this->current = consumingCurrent;
		this->maximumCurrent = this->current;

		// note: to make math easier,
		// waste heat is treated as an 'extra' on top of whatever useful energy consumed.
		this->wasteHeatFraction = 0.13;
	}

	void PowerConsumerModule::Update(GameState& gs, double delta)
	{
		if(this->isActivated())
		{
			auto stn = gs.playerStation;

			stn->lifeSupportSystem->addWasteHeatToSystem(this->getCurrentInAmps()
				* stn->powerSystem->systemVoltage * this->wasteHeatFraction, delta);
		}
	}

	double PowerConsumerModule::getCurrentInAmps()
	{
		return this->current;
	}

	double PowerConsumerModule::getMaximumCurrentInAmps()
	{
		return this->maximumCurrent;
	}

	void PowerConsumerModule::setMaximumCurrentInAmps(double cur)
	{
		this->current = cur;
		this->maximumCurrent = this->current;
	}


	double PowerConsumerModule::rampCurrentTo(double cur)
	{
		return (this->current = std::min(cur, this->getMaximumCurrentInAmps()));
	}

	double PowerConsumerModule::resetCurrent()
	{
		return (this->current = this->maximumCurrent);
	}
}













