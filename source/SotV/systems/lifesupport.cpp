// lifesupport.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sotv/sotv.h"
#include "sotv/systems.h"


// physics constants first
#define GAS_CONSTANT					8.3144
#define STEFAN_BOLTZMANN_CONSTANT		0.000000057
#define MOLAR_MASS_OF_OXYGEN			32
#define MOLAR_MASS_OF_NITROGEN			28
#define SPECIFIC_HEAT_CAPACITY_OF_AIR	1012

// how off can we be.
#define TEMP_THRESHOLD					1.7
#define ATMOS_THRESHOLD					140

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

		/*
			70 kW of cooling on the ISS external thermal control system
		*/

		// subject to 'balans'.
		this->atmosGenerator = new AtmosphereGenerator(9062.5, 40);		// as above
		this->thermalController = new ThermalControl(5600, 4500, 0.91);	// heating, cooling, cooler efficiency (ratio of input/output)

		this->waterConsumer = new PowerConsumerModule(6);
		this->addModule(this->waterConsumer);

		stn->powerSystem->addConsumer(this->waterConsumer);

		// dig in and add these
		{
			this->addModule(this->atmosGenerator->powerConsumer);
			this->addModule(this->thermalController->heatingConsumer);
			this->addModule(this->thermalController->coolingConsumer);
		}






		// for the sake of testing, let's set the mass of nitrogen to be where it needs to be, at the target temperature.
		// use a 79/21 N2/O2 atmosphere.


		// it's coooold (-94 celsius)
		this->internalTemperatureInKelvin = 179;
		// this->internalTemperatureInKelvin = 400;

		{
			// by partial pressure, and that mole ratio == volume ratio,
			// get the partial pressure of N2.
			double pressureN2 = 0.79 * this->targetPressureInPascals;

			double volume = this->station->getInternalVolume();

			// at the target pressure, find out how many moles this is -- PV = nRT.
			double moles = (pressureN2 * volume) / (GAS_CONSTANT * this->targetTemperatureInKelvin);

			this->massOfNitrogenInKilograms = (moles * MOLAR_MASS_OF_NITROGEN) / 1000.0;
		}

		{
			// by partial pressure, and that mole ratio == volume ratio,
			// get the partial pressure of O2.
			double pressureO2 = 0.21 * this->targetPressureInPascals;

			double volume = this->station->getInternalVolume();

			// at the target pressure, find out how many moles this is -- PV = nRT.
			double moles = (pressureO2 * volume) / (GAS_CONSTANT * this->targetTemperatureInKelvin);

			this->massOfOxygenInKilograms = (moles * MOLAR_MASS_OF_OXYGEN) / 1000.0;
		}


		this->massOfOxygenInKilograms *= 4.7;
	}

	void LifeSupportSystem::Render(GameState& gs, double delta, rx::Renderer* ren)
	{
		for(auto m : this->modules)
			m->Render(gs, delta, ren);
	}

	void LifeSupportSystem::addWasteHeatToSystem(double watts, double delta)
	{
		double joules = watts * NS_TO_S(delta);
		double totalGaseousMass = this->massOfNitrogenInKilograms + this->massOfOxygenInKilograms;

		double tempChange = joules / (SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass);
		this->internalTemperatureInKelvin += tempChange;
	}


	void LifeSupportSystem::generateWater(GameState& gs, double delta)
	{
		// nothing
	}

	void LifeSupportSystem::AtmosphereGenerator::updateAtmosphere(Station* stn, LifeSupportSystem* lss, double delta)
	{
		// calculate the change in pressure needed
		double pressureDelta = lss->targetPressureInPascals - lss->getAtmospherePressure();

		// if we're overpressure, cry.
		if(fabs(pressureDelta) >= ATMOS_THRESHOLD || true)
		{
			if(pressureDelta > 0)
			{
				this->powerConsumer->activate();
				// LOG("pressure %f", pressureDelta);

				// ok. PV = nRT -- calculate the change in number of moles. n = PV/RT
				double volume = stn->getInternalVolume();

				double moles = (pressureDelta * volume) / (GAS_CONSTANT * lss->internalTemperatureInKelvin);
				double mass = moles * MOLAR_MASS_OF_OXYGEN;

				// mass is the number of grams of oxygen we need, in grams.
				// energy is... well energy.
				double energyNeeded = mass * this->joulesPerGramOfOxygen;

				// same shit as the heater from here.
				double power = energyNeeded / NS_TO_S(delta);
				double currentNeeded = power / stn->powerSystem->systemVoltage;

				this->powerConsumer->rampCurrentTo(currentNeeded);
				{
					auto ps = stn->powerSystem;

					// consume the energy manually.
					double energy = ps->consumeEnergy(this->powerConsumer->getCurrentInAmps() * ps->systemVoltage, delta);

					double massGenerated = energy / this->joulesPerGramOfOxygen;
					lss->massOfOxygenInKilograms += (massGenerated / 1000.0);
				}
			}
			else
			{
				this->powerConsumer->deactivate();

				// just vent.
				lss->massOfOxygenInKilograms -= 0.30;
			}
		}
		else
		{
			this->powerConsumer->deactivate();
		}
	}

	void LifeSupportSystem::ThermalControl::updateThermalControl(Station* stn, LifeSupportSystem* lss, double delta)
	{
		// the specific heat capacity of air is 1012 joules / kg / K
		// so, first calculate the total amount of atmosphere in the system

		double totalGaseousMass = lss->massOfNitrogenInKilograms + lss->massOfOxygenInKilograms;


		// then, calculate the difference in temperature
		double tempDiff = lss->targetTemperatureInKelvin - lss->internalTemperatureInKelvin;

		if(fabs(tempDiff) >= TEMP_THRESHOLD)
		{
			// calculate how many joules we need.
			double energyNeeded = SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass * tempDiff;

			if(tempDiff > 0)
			{
				this->coolingConsumer->deactivate();

				// this is the instantaneous power (in this tick) we need from the heater.
				double power = energyNeeded / NS_TO_S(delta);

				// ramp the heater to the required current
				double currentNeeded = power / stn->powerSystem->systemVoltage;

				this->heatingConsumer->rampCurrentTo(currentNeeded);
				this->heatingConsumer->activate();
				{
					double energy = stn->powerSystem->consumeEnergy(this->heatingConsumer->getCurrentInAmps()
						* stn->powerSystem->systemVoltage, delta);

					// SHC is 1012 J kg-1 K-1
					// so the change in temperature is energy / (1012 * mass)
					double tempChange = energy / (SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass);
					lss->internalTemperatureInKelvin += tempChange;
				}
			}
			else
			{
				tempDiff *= -1;

				this->heatingConsumer->deactivate();

				// we have the energy needed, so... divide by the efficiency to get the input energy needed
				energyNeeded /= this->coolingEfficiency;
				energyNeeded = fabs(energyNeeded);

				double power = energyNeeded / NS_TO_S(delta);
				double currentNeeded = power / stn->powerSystem->systemVoltage;

				this->coolingConsumer->rampCurrentTo(currentNeeded);
				this->coolingConsumer->activate();
				{
					double energy = stn->powerSystem->consumeEnergy(this->coolingConsumer->getCurrentInAmps()
						* stn->powerSystem->systemVoltage, delta);

					double tempChange = energy / (SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass);
					lss->internalTemperatureInKelvin -= tempChange;
					// LOG("removed %f", tempChange);
				}
			}
		}
		else
		{
			this->heatingConsumer->deactivate();
			this->coolingConsumer->deactivate();
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

		// const double oxygenLeakRateInGrams = 0.4237;
		// const double oxygenLeakRateInGrams = 0.005092608;

		const double oxygenLeakRateInGrams = 0.222222336 + 0.00509;
		this->massOfOxygenInKilograms -= ((oxygenLeakRateInGrams * NS_TO_S(delta)) / 1000.0);

		// clamp
		this->massOfOxygenInKilograms = std::max(0.0, this->massOfOxygenInKilograms);


		// radiate some heat. emi
		const double emissivity = 0.89;
		const double surfaceArea = 60 * 8;

		const double powerLoss = emissivity * STEFAN_BOLTZMANN_CONSTANT * surfaceArea * pow(this->internalTemperatureInKelvin, 4);
		const double jouleLoss = powerLoss * NS_TO_S(delta);

		double totalGaseousMass = this->massOfOxygenInKilograms + this->massOfNitrogenInKilograms;
		this->internalTemperatureInKelvin -= jouleLoss / (SPECIFIC_HEAT_CAPACITY_OF_AIR * totalGaseousMass);

		for(auto m : this->modules)
			m->Update(gs, delta);


		// here, we need to smartly activate and deactivate each component of the LSS depending on the environment.
		if(this->isActivated())
		{
			this->atmosGenerator->updateAtmosphere(this->station, this, delta);
			this->thermalController->updateThermalControl(this->station, this, delta);
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

















