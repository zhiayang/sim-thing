// Font.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <tuple>

#include <stdio.h>
#include <sys/stat.h>

#include "imgui.h"
#include "graphicswrapper.h"

namespace Rx
{
	typedef std::tuple<std::string, int> FontTuple;
	static std::map<FontTuple, Font> fontMap;

	Font getFont(std::string name, int size, bool hinting)
	{
		// see long comment on fonts in main.cpp
		size *= 2;

		FontTuple tup(name, size);
		if(fontMap.find(tup) != fontMap.end())
		{
			return fontMap[tup];
		}

		std::string path = AssetLoader::getResourcePath() + "fonts/" + name + ".ttf";


		uint8_t* ttfbuffer = 0;
		size_t ttfSize = 0;
		{
			struct stat st;
			int err = stat(path.c_str(), &st);
			if(err != 0) ERROR("fstat failed with: errno = %d\n", errno);

			size_t expected = st.st_size;
			FILE* f = fopen(path.c_str(), "r");

			ttfbuffer = new uint8_t[expected];

			size_t done = fread(ttfbuffer, 1, expected, f);
			if(done != expected) ERROR("failure in reading file: expected %zu, got %zu bytes (errno = %d)\n", expected, done, errno);

			ttfSize = done;

			fclose(f);
		}

		// this is because imgui deletes the buffer
		uint8_t* bufcopy = new uint8_t[ttfSize];
		memcpy(bufcopy, ttfbuffer, ttfSize);

		ImFont* imguif = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(bufcopy, (int) ttfSize, size);

		Font fnt = Font(name);
		fnt.imgui = imguif;

		fnt.ttfBuffer = ttfbuffer;
		fnt.ttfBufferSize = ttfSize;

		fontMap[tup] = fnt;

		return fnt;
	}

	void closeAllFonts()
	{
		for(auto pair : fontMap)
		{
			pair.second.imgui->Clear();
			delete[] pair.second.ttfBuffer;
		}
	}
}





