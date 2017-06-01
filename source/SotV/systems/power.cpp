// power.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"
#include "sotv/systems.h"

#define SYSTEM_VOLTAGE	240

namespace Sotv
{
	PowerSystem::PowerSystem(Station* stn) : System(stn), systemVoltage(SYSTEM_VOLTAGE) { }

	void PowerSystem::Render(GameState& gs, double delta, rx::Renderer* ren)
	{
	}

	void PowerSystem::Update(GameState& gs, double delta)
	{
		// reset for this tick.
		this->totalConsumed = 0;

		// get the total generated in this update (make it double, because we're doing bad scaling stuff)
		double total = 0;
		for(auto g : this->generators)
			total += g->getProductionInWatts();

		// scale by the delta
		total *= NS_TO_S(delta);
		total += this->previousExcess;

		// todo: priority system of some kind. right now, fill cells in parallel.
		assert(!this->storage.empty());

		double startedWith = total;

		#if 0
			double avgFill = total / (double) this->storage.size();
		#else
			double avgFill = total;
		#endif

		for(auto s : this->storage)
		{
			total -= s->storeEnergy(avgFill);
			if(total <= 0) break;
		}

		// add it back, to be tried again next tick
		// but once it's full we just give up. dump it.
		if(total < startedWith)
			this->previousExcess += total;

		else
			this->previousExcess = 0;



		// ok, consume power now
		for(auto cons : this->consumers)
		{
			if(cons->isActivated())
			{
				// P = IV -- get power by multiplying with system_voltage,
				// then get energy by multiplying by the delta time.

				double watts = cons->getCurrentInAmps() * this->systemVoltage;
				this->consumeEnergy(watts, delta);
			}
		}
	}

	double PowerSystem::consumeEnergy(double watts, double delta)
	{
		double needed = watts* NS_TO_S(delta);
		double consumed = 0;

		// consume from cells in series
		for(auto st : this->storage)
		{
			consumed += st->drainEnergy(needed - consumed);

			if(consumed >= needed)
				break;
		}

		if(consumed < needed)
			LOG("Short of %.2f joules this tick", needed - consumed);

		// totalConsumed works on watts, not joules per delta.
		// so, we divide by delta when we add.
		this->totalConsumed += (consumed / NS_TO_S(delta));
		return consumed;
	}


	double PowerSystem::getTotalProductionInWatts()
	{
		double total = 0;
		for(auto g : this->generators)
			total += g->getProductionInWatts();

		return total;
	}

	double PowerSystem::getTotalConsumptionInWatts()
	{
		return this->totalConsumed;
	}

	double PowerSystem::getTotalStorageInJoules()
	{
		double total = 0;
		for(auto s : this->storage)
			total += s->getEnergyInJoules();

		return total;
	}

	double PowerSystem::getTotalCapacityInJoules()
	{
		double total = 0;
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

	void PowerSystem::addConsumer(PowerConsumerModule* cons)
	{
		assert(cons);
		if(std::find(this->consumers.begin(), this->consumers.end(), cons) != this->consumers.end())
			ERROR("Consumer (ptr '%p') already exists in the system, skipping", cons);

		else
			this->consumers.push_back(cons), this->addModule(cons);
	}
}











