// lifesupport.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"
#include "sotv/systems.h"


// physics constants first
#define GAS_CONSTANT					8.31445
#define MOLAR_MASS_OF_OXYGEN			32
#define MOLAR_MASS_OF_NITROGEN			28
#define SPECIFIC_HEAT_CAPACITY_OF_AIR	1012

// how off can we be.
#define HEATER_THRESHOLD				0.8
#define ATMOS_THRESHOLD					80

namespace Sotv
{
	// coding note:
	// we can break up this LSS into multiple subclasses (ie. components), but we won't for now
	// it just results in a lot of boilerplate.
	LifeSupportSystem::LifeSupportSystem(Station* stn, double targetPressure, double targetTemperature) : System(stn),
		targetPressureInPascals(targetPressure), targetTemperatureInKelvin(targetTemperature)
	{
		// elektron consumes 1L (of water) / crew / day for 1 kW
		// let's fudge this to like 0.8L / crew / day

		/*
			some math:
			let's say we need 1.6 kg of oxygen per person per day
			that's like 50 mol per person per day

			so consumption is 0.000578704 mol/s per person

			if we assume a design maximum of 12 people for this theoretical 'station', we should be able to produce
			about 0.006944448, say 0.008, moles per second (at a maximum).

			a practical amount of energy -- 287 kJ per mole of O2, from wikipedia
			make that a nice 290 kJ per mole.

			that means we need 290000 J per mole, or 9062.5 J per gram of O2.
			we need 0.008 moles per second, so 2320 J per second. that means we need 9.67 amps to break even.
			make it 16 for a safe margin.

			this means we should be able to produce enough oxygen to maintain constant pressure with 12 people on board.
			theoretical max power: 3840 W
			this gives us +.424g O2 per second, max.
		*/

		// subject to 'balans'.
		this->joulesPerGramOfOxygen = 9062.5;						// as above
		this->atmosphereConsumer = new PowerConsumerModule(16);		// as above
		this->heatingConsumer = new PowerConsumerModule(400000);
		this->waterConsumer = new PowerConsumerModule(6);

		this->addModule(this->atmosphereConsumer);
		this->addModule(this->heatingConsumer);
		this->addModule(this->waterConsumer);

		stn->powerSystem->addConsumer(this->atmosphereConsumer);
		stn->powerSystem->addConsumer(this->heatingConsumer);
		stn->powerSystem->addConsumer(this->waterConsumer);

		// for the sake of testing, let's set the mass of nitrogen to be where it needs to be, at the target temperature.
		// use a 79/21 N2/O2 atmosphere.


		// it's coooold (-94 celsius)
		// this->internalTemperatureInKelvin = 179;
		this->internalTemperatureInKelvin = 297;

		{
			// by partial pressure, and that mole ratio == volume ratio,
			// get the partial pressure of N2.
			double pressureN2 = 0.79 * this->targetPressureInPascals;

			double volume = this->station->getInternalVolume();

			// at the target pressure, find out how many moles this is -- PV = nRT.
			double moles = (pressureN2 * volume) / (GAS_CONSTANT * this->targetTemperatureInKelvin);

			this->massOfNitrogenInKilograms = (moles * MOLAR_MASS_OF_NITROGEN) / 1000.0;
		}

		this->massOfOxygenInKilograms = 0;


		{
			// by partial pressure, and that mole ratio == volume ratio,
			// get the partial pressure of O2.
			double pressureO2 = 0.21 * this->targetPressureInPascals;

			double volume = this->station->getInternalVolume();

			// at the target pressure, find out how many moles this is -- PV = nRT.
			double moles = (pressureO2 * volume) / (GAS_CONSTANT * this->targetTemperatureInKelvin);

			this->massOfOxygenInKilograms = (moles * MOLAR_MASS_OF_OXYGEN) / 1000.0;
		}
	}

	void LifeSupportSystem::Render(GameState& gs, double delta, Rx::Renderer* ren)
	{
		for(auto m : this->modules)
			m->Render(gs, delta, ren);
	}




	void LifeSupportSystem::generateWater(GameState& gs, double delta)
	{
		// nothing
	}

	void LifeSupportSystem::generateOxygen(GameState& gs, double delta)
	{
		// in the previous tick, the oxygen generator was possibly on.
		if(this->atmosphereConsumer->isActivated())
		{
			// it was on. calculate the changes.
			// assumes the previous delta was the same as this delta, which is always true based on how we do updates.
			double energy = this->atmosphereConsumer->getCurrentInAmps() * this->station->powerSystem->systemVoltage * NS_TO_S(delta);
			double massGenerated = energy / this->joulesPerGramOfOxygen;

			this->massOfOxygenInKilograms += (massGenerated / 1000.0);

			// LOG("generated %fg of O2 / %f", massGenerated, energy);
		}


		// calculate the change in pressure needed
		double pressureDelta = this->targetPressureInPascals - this->getAtmospherePressure();

		// if we're overpressure, cry.
		if(fabs(pressureDelta) >= ATMOS_THRESHOLD || true)
		{
			this->atmosphereConsumer->activate();
			if(pressureDelta > 0)
			{
				// LOG("pressure %f", pressureDelta);

				// ok. PV = nRT -- calculate the change in number of moles. n = PV/RT
				double volume = this->station->getInternalVolume();

				double moles = (pressureDelta * volume) / (GAS_CONSTANT * this->internalTemperatureInKelvin);
				double mass = moles * MOLAR_MASS_OF_OXYGEN;

				// mass is the number of grams of oxygen we need, in grams.
				// energy is... well energy.
				double energyNeeded = mass * this->joulesPerGramOfOxygen;

				// same shit as the heater from here.
				double power = energyNeeded / NS_TO_S(delta);
				double currentNeeded = power / this->station->powerSystem->systemVoltage;
				this->atmosphereConsumer->rampCurrentTo(currentNeeded);
			}
		}
		else
		{
			this->atmosphereConsumer->deactivate();
		}
	}

	void LifeSupportSystem::generateHeat(GameState& gs, double delta)
	{
		// the specific heat capacity of air is 1012 joules / kg / K
		// so, first calculate the total amount of atmosphere in the system

		double totalGaseousMass = this->massOfNitrogenInKilograms + this->massOfOxygenInKilograms;

		// in the previous tick, the heater was possibly on.
		if(this->heatingConsumer->isActivated())
		{
			// it was on. calculate the changes.
			// assumes the previous delta was the same as this delta, which is always true based on how we do updates.
			double energy = this->heatingConsumer->getCurrentInAmps() * this->station->powerSystem->systemVoltage * NS_TO_S(delta);

			// SHC is 1012 J kg-1 K-1
			// so the change in temperature is energy / (1012 * mass)
			double tempChange = energy / (SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass);
			this->internalTemperatureInKelvin += tempChange;
		}



		// then, calculate the difference in temperature
		double tempDiff = this->targetTemperatureInKelvin - this->internalTemperatureInKelvin;

		if(fabs(tempDiff) >= HEATER_THRESHOLD)
		{
			this->heatingConsumer->activate();

			// if we're hotter, err... cry?
			if(tempDiff > 0)
			{
				// calculate how many joules we need.
				double energyNeeded = SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass * tempDiff;

				// this is the instantaneous power (in this tick) we need from the heater.
				double power = energyNeeded / NS_TO_S(delta);

				// ramp the heater to the required current
				double currentNeeded = power / this->station->powerSystem->systemVoltage;
				this->heatingConsumer->rampCurrentTo(currentNeeded);
			}
		}
		else
		{
			this->heatingConsumer->deactivate();
		}
	}

	void LifeSupportSystem::Update(GameState& gs, double delta)
	{
		// leak some pressure.
		// ISS leaks at about 180 dm3 per day
		// make that a good 330 for us.
		// = 13.75 mol per day = 0.000159144 mol per second

		// for now, let's assume N2 never leaks, and only O2 leaks.
		// that loses us 0.005092608g per second

		// const double oxygenLeakRateInGrams = 50.92608;
		// const double oxygenLeakRateInGrams = 0.005092608;

		// double oxygenLeakRateInGrams = 0.222222336 + 0.00509;
		double oxygenLeakRateInGrams = 0.4237;
		this->massOfOxygenInKilograms -= ((oxygenLeakRateInGrams * NS_TO_S(delta)) / 1000.0);

		// clamp
		this->massOfOxygenInKilograms = std::max(0.0, this->massOfOxygenInKilograms);
		// LOG("mass O2 = %f", this->massOfOxygenInKilograms);

		for(auto m : this->modules)
			m->Update(gs, delta);


		// here, we need to smartly activate and deactivate each component of the LSS depending on the environment.
		if(this->isActivated())
		{
			this->generateHeat(gs, delta);
			this->generateOxygen(gs, delta);
		}
	}

	double LifeSupportSystem::getAtmosphereTemperature()
	{
		return this->internalTemperatureInKelvin;
	}

	double LifeSupportSystem::getAtmospherePressure()
	{
		// PV = nRT, P = (nRT)/V
		double moles = ((this->massOfOxygenInKilograms * 1000) / MOLAR_MASS_OF_OXYGEN)
			+ ((this->massOfNitrogenInKilograms * 1000) / MOLAR_MASS_OF_NITROGEN);

		double volume = this->station->getInternalVolume();

		return (moles * GAS_CONSTANT * this->internalTemperatureInKelvin) / volume;
	}

	// we need to override this, because we should toggle all the systems on/off at the same time, but since each module
	// can be independently (and automatically) toggled based on the pressure/temperature of the station, we can't just
	// toggle all of them.
	bool LifeSupportSystem::toggle()
	{
		this->activated = !this->activated;
		for(auto m : this->getModules())
		{
			if(this->isActivated())
				m->activate();

			else
				m->deactivate();
		}

		return this->isActivated();
	}
}

















