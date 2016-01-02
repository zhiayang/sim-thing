// Logging.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string>

#include "SDL2/SDL.h"

#include "utilities.h"

namespace Logging
{
	static const char* LevelStrings[]			= { "Info", "Warning", "Critical" };
	static const int NUM_LEVELS					= sizeof(LevelStrings);
	static const int EXIT_THRESHOLD				= 2;
	static std::shared_ptr<Logger> globalLogger(new Logger());

	std::shared_ptr<Logger> GetGlobalLoggerInstance()
	{
		return globalLogger;
	}












	static void LogGeneric(int level, FILE* outstr, const char* fmt, va_list args)
	{
		assert(level < NUM_LEVELS);
		std::string head = std::string("[") + LevelStrings[level] + std::string("]");

		auto outfile = outstr ? outstr : (level == 0 ? stdout : stderr);

		char* fmtted = 0;
		vasprintf(&fmtted, fmt, args);


		// handle sdl error shit
		std::string sdlerr;
		if(level >= 1)
			sdlerr = std::string(" (SDL Error: ") + SDL_GetError() + ")";


		fprintf(outfile, "%s: %s%s\n", head.c_str(), fmtted, sdlerr.empty() ? "" : sdlerr.c_str());

		if(level >= EXIT_THRESHOLD)
		{
			SDL_Event quitevent;
			quitevent.type = SDL_QUIT;
			SDL_PushEvent(&quitevent);
		}
	}

	void Logger::Info(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		LogGeneric(0, 0, fmt, ap);
		va_end(ap);
	}

	void Logger::Warn(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		LogGeneric(1, 0, fmt, ap);
		va_end(ap);
	}

	void Logger::Error(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		LogGeneric(2, 0, fmt, ap);
		va_end(ap);
	}
}
