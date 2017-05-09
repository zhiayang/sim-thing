// powermodules.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"

namespace Sotv
{
	PowerGenModule::PowerGenModule() { }
	PowerStorageModule::PowerStorageModule() { }


	SolarGenModule::SolarGenModule(size_t watts)
	{
		this->productionInWatts = watts;
	}

	size_t SolarGenModule::getProductionInWatts()
	{
		return this->productionInWatts;
	}





	LithiumBatteryModule::LithiumBatteryModule(size_t initialStorage, size_t cap)
	{
		this->current = initialStorage;
		this->capacity = cap;

		LOG("%zu / %zu", this->current, this->capacity);
		assert(this->current <= this->capacity);
	};

	size_t PowerStorageModule::getEnergyInJoules()
	{
		return this->current;
	}

	size_t PowerStorageModule::getCapacityInJoules()
	{
		return this->capacity;
	}

	size_t PowerStorageModule::storeEnergy(size_t joules)
	{
		size_t left = this->capacity - this->current;
		this->current = std::min(this->capacity, this->current + joules);

		return std::min(joules, left);
	}

	size_t PowerStorageModule::drainEnergy(size_t joules)
	{
		if(this->current >= joules)
		{
			this->current -= joules;
			return joules;
		}
		else
		{
			size_t ret = this->current;
			this->current = 0;

			return ret;
		}
	}
}













