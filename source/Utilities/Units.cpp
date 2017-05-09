// Units.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "tinyformat.h"
#include "utilities.h"

namespace Util
{
	#define NUM_SMALL_PREFIXES	6
	#define NUM_LARGE_PREFIXES	5

	static const char* smallPrefixes[] = { "f", "p", "n", "µ", "m", "" };
	static const char* largePrefixes[5] = { "P", "T", "G", "M", "k" };

	// const size_t upperThreshold = 5;
	// const double lowerThreshold = 0.5;

	// std::string formatWithUnits(size_t number, std::string unit)
	// {
	// 	double lg = round(log10((double) number));
	// 	if(lg >= 0 && lg <= 1) return tfm::format("%zu%s", number, unit);

	// 	if(lg < 0)
	// 	{
	// 		return tfm::format("%zu%s%s", number, smallPrefixes[sizeof(smallPrefixes) - ((size_t) lg / 3)], unit);
	// 	}
	// 	else
	// 	{
	// 		return tfm::format("%zu%s%s", number, largePrefixes[sizeof(largePrefixes) - ((size_t) lg / 3)], unit);
	// 	}
	// }

	std::string formatWithUnits(double number, int prec, std::string unit)
	{
		double lg = log10(number);
		auto si = 3 * ((ssize_t) lg / 3);

		if(lg >= 0 && lg <= 1) return tfm::format("%.*f%s", prec, number, unit);

		// // magic fudge factor -- try to give nicer numbers like 40 MJ vs 0.04 GJ
		// if(fabs(si - lg) > 0.8)
		// 	si += (si > lg ? -1 : +1) * 3;

		if(lg < 0)
		{
			return tfm::format("%.*f%s%s", prec, number * pow(10, si), smallPrefixes[NUM_SMALL_PREFIXES - (si / 3)], unit);
		}
		else
		{
			LOG("%f, %f, %zu", lg, log10(number), si);
			return tfm::format("%.*f%s%s", prec, number / pow(10, si), largePrefixes[NUM_LARGE_PREFIXES - (si / 3)], unit);
		}
	}
}












