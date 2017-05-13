// powermodules.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"

namespace Sotv
{
	PowerGenModule::PowerGenModule() { }
	PowerStorageModule::PowerStorageModule() { }


	SolarGenModule::SolarGenModule(double watts)
	{
		this->productionInWatts = watts;
	}

	double SolarGenModule::getProductionInWatts()
	{
		return this->productionInWatts;
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













