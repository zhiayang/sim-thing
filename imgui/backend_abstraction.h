// backend_abstraction.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

namespace ImGuiBackend
{
	enum class CursorType
	{
		Arrow,
		IBeam,
		Move,
		ResizeEW,
		ResizeNS,
		ResizeNESW,	// northeast, southwest
		ResizeNWSE,	// northwest, southeast
		ResizeAll,
	};

	void SetCursor(CursorType type);
}
