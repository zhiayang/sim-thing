
// Util.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <stdio.h>

#include "utilities.h"
#include "sdlwrapper.h"

#include "imgui.h"

#include <SDL2/SDL_ttf.h>

namespace Util
{
	namespace Random
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


	typedef std::tuple<std::string, int> FontTuple;
	static std::map<FontTuple, Font> fontMap;

	Font getFont(std::string name, int size, bool hinting)
	{
		FontTuple tup(name, size);
		if(fontMap.find(tup) != fontMap.end())
		{
			return fontMap[tup];
		}

		std::string path = AssetLoader::getResourcePath() + "fonts/" + name + ".ttf";

		ImFont* imguif = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size);
		TTF_Font* sdlf = TTF_OpenFont(path.c_str(), size);

		Font f { imguif, sdlf };
		fontMap[tup] = f;

		return f;
	}

	void closeAllFonts()
	{
		for(auto pair : fontMap)
		{
			pair.second.imgui->Clear();
			TTF_CloseFont(pair.second.sdl);
		}
	}
}













