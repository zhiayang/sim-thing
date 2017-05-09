// Station.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <algorithm>

#include <util.h>
#include <sotv/sotv.h>

namespace Sotv
{
	Station* Station::makeDefaultSpaceStation(std::string name)
	{
		auto stn = new Station();

		stn->stationName = name;
		LOG("Creating new (default) station with name '%s'", name.c_str());

		LOG("Creating power system");
		stn->powerSystem = new PowerSystem();
		stn->addSystem(stn->powerSystem);

		LOG("Adding 4x 600W solar panels");
		for(int i = 0; i < 4; i++)
		{
			auto panel = new SolarGenModule(600);
			stn->powerSystem->addGenerator(panel);
		}

		LOG("Adding 1x 50MJ battery");
		stn->powerSystem->addStorage(new LithiumBatteryModule(0, (size_t) 50000 * 1000 * 1000));

		return stn;
	}



	void Station::addSystem(System* sys)
	{
		assert(sys);
		if(std::find(this->systems.begin(), this->systems.end(), sys) != this->systems.end())
			ERROR("System (ptr '%p') already exists in the station, skipping", sys);

		else
			this->systems.push_back(sys);
	}

	void Station::Render(GameState& gs, float delta, Rx::Renderer* ren)
	{
		for(auto s : this->systems)
			s->Render(gs, delta, ren);
	}

	void Station::Update(GameState& gs, float delta)
	{
		for(auto s : this->systems)
			s->Update(gs, delta);
	}
}
