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

		// 800 cubic metres.
		stn->setInternalVolume(350);

		stn->stationName = name;
		LOG("Creating new (default) station with name '%s'", name.c_str());

		LOG("Creating power system");
		stn->powerSystem = new PowerSystem(stn);
		stn->addSystem(stn->powerSystem);

		LOG("Adding 4x 600 W solar panels");
		for(int i = 0; i < 4; i++)
		{
			auto panel = new SolarGenModule(6000000000);
			stn->powerSystem->addGenerator(panel);
		}

		LOG("Adding 3x 72 Ah batteries");
		for(int i = 0; i < 3; i++)
			stn->powerSystem->addStorage(new LithiumBatteryModule(0, Units::convertAmpHoursJoules(72, stn->powerSystem->systemVoltage)));

		LOG("Adding Life Support system");
		{
			// specs stolen from ISS oxygen-generating-system
			// 10-55A, ~3500W
			// say 40A, 2400W, gives operating 60V

			auto lss = new LifeSupportSystem(stn, 101400, Units::convertCelsiusToKelvin(24));
			stn->lifeSupportSystem = lss;

			stn->addSystem(lss);
		}



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

	void Station::Render(GameState& gs, double delta, Rx::Renderer* ren)
	{
		for(auto s : this->systems)
			s->Render(gs, delta, ren);
	}

	void Station::Update(GameState& gs, double delta)
	{
		for(auto s : this->systems)
			s->Update(gs, delta);
	}
}
