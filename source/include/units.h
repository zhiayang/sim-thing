// units.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <string>

namespace Units
{
	std::string formatWithUnits(double number, int prec, std::string unit);

	double convertJoulesToAmpHours(double joules, double ahs);
	double convertAmpHoursToJoules(double ahs, double joules);

	double convertJoulesToWattHours(double joules);
	double convertWattHoursToJoules(double whs);

	double convertKelvinToCelsius(double kelvin);
	double convertCelsiusToKelvin(double celsius);
}
