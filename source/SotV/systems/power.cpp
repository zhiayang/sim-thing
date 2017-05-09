// power.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"
#include "sotv/systems.h"

namespace Sotv
{
	void PowerSystem::Render(GameState& gs, float delta, Rx::Renderer* ren)
	{
	}

	void PowerSystem::Update(GameState& gs, float delta)
	{
		// get the total generated in this update (make it double, because we're doing bad scaling stuff)
		double total = 0;
		for(auto g : this->generators)
			total += g->getProductionInWatts();

		// scale by the delta
		total *= (delta / 1000.0 / 1000.0 / 1000.0);

		// todo: priority system of some kind. right now, fill cells in series.
		assert(!this->storage.empty());

		for(auto s : this->storage)
			total -= s->storeEnergy(total);

		if(total != 0)
			LOG("Wasted %f joules this tick", total);
	}



	size_t PowerSystem::getTotalProductionInWatts()
	{
		size_t total = 0;
		for(auto g : this->generators)
			total += g->getProductionInWatts();

		return total;
	}

	size_t PowerSystem::getTotalStorageInJoules()
	{
		size_t total = 0;
		for(auto s : this->storage)
			total += s->getEnergyInJoules();

		return total;
	}

	size_t PowerSystem::getTotalCapacityInJoules()
	{
		size_t total = 0;
		for(auto s : this->storage)
			total += s->getCapacityInJoules();

		return total;
	}




	void PowerSystem::addGenerator(PowerGenModule* gen)
	{
		assert(gen);
		if(std::find(this->generators.begin(), this->generators.end(), gen) != this->generators.end())
			ERROR("Generator (ptr '%p') already exists in the system, skipping", gen);

		else
			this->generators.push_back(gen), this->addModule(gen);
	}

	void PowerSystem::addStorage(PowerStorageModule* stor)
	{
		assert(stor);
		if(std::find(this->storage.begin(), this->storage.end(), stor) != this->storage.end())
			ERROR("Storage (ptr '%p') already exists in the system, skipping", stor);

		else
			this->storage.push_back(stor), this->addModule(stor);
	}
}
