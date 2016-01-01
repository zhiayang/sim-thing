// AssetLoader.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <string>
#include <assert.h>
#include <sys/stat.h>

#include "SDL2/SDL.h"

#include "utilities.h"
#include "assetloader.h"

namespace AssetLoader
{
	Asset* Load(const char* path)
	{
		std::string binPath = SDL_GetBasePath();
		assert(!binPath.empty());

		binPath += "../assets/";
		binPath += path;

		Asset* ass = new Asset();
		ass->path = binPath;

		// read the file.
		FILE* file = fopen(ass->path.c_str(), "r");
		if(file == nullptr)
			ERROR("Failed to read asset '%s' (fopen returned null)", ass->path.c_str());

		struct stat s;
		fstat(fileno(file), &s);

		ass->raw = new uint8_t[s.st_size];
		off_t read = fread(ass->raw, 1, s.st_size, file);

		if(read != s.st_size)
			ERROR("Failed to read whole asset from file '%s'", ass->path.c_str());

		ass->sdlrw = SDL_RWFromConstMem(ass->raw, read);

		fclose(file);

		auto idx = ass->path.rfind('.');
		std::string ext;

		if(idx != std::string::npos)
		    ext = ass->path.substr(idx + 1);


		// check.
		if(ext == "png")	ass->type = AssetType::ImagePNG;
		else				ass->type = AssetType::Unknown;

		LOG("Loaded asset '%s'", ass->path.c_str());
		return ass;
	}

	void Unload(Asset* ass)
	{
		SDL_FreeRW(ass->sdlrw);
		delete ass->raw;
		delete ass;
	}

	std::string getResourcePath()
	{
		return std::string(SDL_GetBasePath()) + "../assets/";
	}
}





