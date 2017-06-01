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

		LOG("Adding 8x 7 kW solar panels");
		for(int i = 0; i < 8; i++)
		{
			auto panel = new SolarGenModule(34 * 12, 0.29);
			stn->powerSystem->addGenerator(panel);
		}

		LOG("Adding 3x 14.5 kWh batteries");
		for(int i = 0; i < 3; i++)
		{
			auto joules = Units::convertWattHoursToJoules(14.5 * 1000.0);

			double fill = util::random::get(0.001, 0.3);
			stn->powerSystem->addStorage(new LithiumBatteryModule(fill * joules, joules));
		}

		LOG("Adding Life Support system");
		{
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

	void Station::Render(GameState& gs, double delta, rx::Renderer* ren)
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
