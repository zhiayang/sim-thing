// Resources.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include "controller.h"

namespace Rsrc
{
	struct Resource
	{
		virtual ~Resource() = 0;
	};

	struct Bitmap : Resource
	{
		Bitmap(const char* path);
		virtual ~Bitmap() override;
	};
}
