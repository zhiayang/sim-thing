
// Util.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <stdio.h>

#include "rx.h"
#include "utilities.h"

namespace util
{
	namespace random
	{
		static std::random_device g;
		static std::uniform_real_distribution<double> d(-1.0, 1.0);

		double get(double scale)
		{
			return d(g) * scale;
		}

		double get(double lower, double upper)
		{
			std::uniform_real_distribution<double> something(lower, upper);
			return something(g);
		}
	}
}












