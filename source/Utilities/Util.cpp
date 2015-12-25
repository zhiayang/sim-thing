
// Util.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <stdio.h>

#include "utilities.h"
#include "sdlwrapper.h"

#include "imgui.h"

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

	namespace Font
	{
		typedef std::tuple<std::string, int> FontTuple;
		static std::map<FontTuple, ImFont*> fontMap;

		ImFont* get(std::string name, int size, bool hinting)
		{
			FontTuple tup(name, size);
			if(fontMap.find(tup) != fontMap.end())
			{
				return fontMap[tup];
			}

			// SDL::Font* f = new SDL::Font(AssetLoader::getResourcePath() + "fonts/" + name + ".ttf", size, hinting);
			ImFont* f = ImGui::GetIO().Fonts->AddFontFromFileTTF((AssetLoader::getResourcePath() + "fonts/" + name + ".ttf").c_str(), size);
			fontMap[tup] = f;

			return f;
		}

		void closeAll()
		{
			for(auto pair : fontMap)
			{
				assert(pair.second);
				pair.second->Clear();
			}
		}
	}
}













