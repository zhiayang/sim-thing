// sdl_backend_impl.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "backend_abstraction.h"
#include "graphicswrapper.h"

#include <map>

namespace ImGuiBackend
{
	std::map<CursorType, SDL_Cursor*> cursors;
	void SetCursor(CursorType type)
	{
		if(cursors.find(type) == cursors.end())
		{
			switch(type)
			{
				case CursorType::Arrow:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
					break;

				case CursorType::IBeam:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
					break;

				case CursorType::Move:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
					break;

				case CursorType::ResizeEW:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
					break;

				case CursorType::ResizeNS:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
					break;

				case CursorType::ResizeNSEW:
					cursors[type] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
					break;
			}
		}

		if(SDL_GetCursor() != cursors[type])
			SDL_SetCursor(cursors[type]);
	}
}



























