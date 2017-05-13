// Units.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <array>

#include "tinyformat.h"
#include "utilities.h"

namespace Units
{
	#define NUM_SMALL_PREFIXES	5
	#define NUM_LARGE_PREFIXES	5

	static const std::array<std::string, NUM_SMALL_PREFIXES> smallPrefixes = {{ "f", "p", "n", "µ", "m" }};
	static const std::array<std::string, NUM_LARGE_PREFIXES> largePrefixes = {{ "P", "T", "G", "M", "k" }};
	static const std::array<std::string, 4> foo = {{ "x", "x", "x", "x" }};

	std::string formatWithUnits(double number, int prec, std::string unit)
	{
		if(number == 0)
			return tfm::format("%.*f %s", prec, number, unit);

		double negmul = (number < 0) ? -1 : 1;
		number = fabs(number);

		// printf("num %f, %s\n", number, unit.c_str());

		double lg = log10(number);
		ssize_t si = (ssize_t) lg;

		// printf("si = %zd\n", si);

		// make it prefer > 1 numbers (rather than 0.01 mW, for instance, give 10 µW)
		if(si > 0)	si = si - (si % 3);
		else		si = (si - 2) + (labs(si - 2) % 3);

		if(si == 0) return tfm::format("%.*f %s", prec, negmul * number, unit);


		if(lg < 0)
		{
			// printf("[si = %zd]\n\n\n", si);
			return tfm::format("%.*f %s%s", prec, negmul * number * pow(10, -si), smallPrefixes[NUM_SMALL_PREFIXES - (-1 * si / 3)], unit);
		}
		else
		{
			// auto index = NUM_LARGE_PREFIXES - (si / 3);
			// assert(index >= 0);
			// assert(index < NUM_LARGE_PREFIXES);

			// printf("[si = %zd, %zu]\n\n\n", si, index);
			return tfm::format("%.*f %s%s", prec, negmul * number / pow(10, si), largePrefixes[NUM_LARGE_PREFIXES - (si / 3)], unit);
		}
	}


	#define SECONDS_PER_HOUR	3600.0

	double convertJoulesToAmpHours(double joules, double voltage)
	{
		// divide by system voltage to get a number in amp-seconds.
		// we divide by 3600 to convert from amp-seconds to amp-hours

		return (joules / (voltage * SECONDS_PER_HOUR));
	}

	double convertAmpHoursToJoules(double ahs, double voltage)
	{
		// do the reverse
		return ahs * voltage * SECONDS_PER_HOUR;
	}

	double convertJoulesToWattHours(double joules)
	{
		return joules / SECONDS_PER_HOUR;
	}

	double convertWattHoursToJoules(double whs)
	{
		return whs * SECONDS_PER_HOUR;
	}

	double convertKelvinToCelsius(double kelvin)
	{
		return kelvin - 273.15;
	}
	double convertCelsiusToKelvin(double celsius)
	{
		return celsius + 273.15;
	}
}












