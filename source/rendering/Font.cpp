// Font.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <algorithm>

#include <stdio.h>
#include <sys/stat.h>

#include "renderer/rx.h"

#include <glbinding/gl/gl.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_pack_rect.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace Rx
{
	typedef std::pair<std::string, size_t> FontTuple;
	static std::map<FontTuple, Font*> fontMap;

	Font* getFont(std::string name, size_t pixelSize, uint32_t firstChar, size_t numChars, size_t oversampleH, size_t oversampleV)
	{
		// see long comment on fonts in main.cpp

		FontTuple tup(name, pixelSize);
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
		// uint8_t* bufcopy = new uint8_t[ttfSize];
		// memcpy(bufcopy, ttfbuffer, ttfSize);

		Font* font = new Font(name);
		font->pixelSize = pixelSize;

		font->ttfBuffer = ttfbuffer;
		font->ttfBufferSize = ttfSize;

		font->firstChar = firstChar;
		font->numChars = numChars;

		font->horzOversample = oversampleH;
		font->vertOversample = oversampleV;

		// determine how large, approximately, the atlas needs to be.
		size_t maxpixsize = std::max(oversampleH * pixelSize, oversampleV * pixelSize);
		size_t numperside = (size_t) (sqrt(numChars) + 1);

		font->atlasHeight = numperside * maxpixsize;
		font->atlasWidth = numperside * maxpixsize;

		font->atlas = new uint8_t[font->atlasWidth * font->atlasHeight];
		assert(font->atlas && "failed to allocate memory for atlas");
		memset(font->atlas, 0, font->atlasWidth * font->atlasHeight);


		LOG("Allocated a %s font atlas (%zux%zu) for font '%s'", Units::formatWithUnits(font->atlasWidth * font->atlasHeight, 1, "B").c_str(),
			font->atlasWidth, font->atlasHeight, name.c_str());


		stbtt_InitFont(&font->fontInfo, font->ttfBuffer, 0);


		// begin stb_truetype stuff
		{
			stbtt_pack_context context;
			if(!stbtt_PackBegin(&context, font->atlas, font->atlasWidth, font->atlasHeight, 0, 1, nullptr))
				ERROR("Failed to initialise font '%s'", name.c_str());

			font->charInfo = new stbtt_packedchar[font->numChars];
			memset(font->charInfo, 0, sizeof(stbtt_packedchar) * font->numChars);

			stbtt_PackSetOversampling(&context, font->horzOversample, font->vertOversample);

			if(!stbtt_PackFontRange(&context, font->ttfBuffer, 0, pixelSize, font->firstChar,
				font->numChars, (stbtt_packedchar*) font->charInfo))
			{
				ERROR("Failed to pack font '%s' into atlas", name.c_str());
			}

			stbtt_PackEnd(&context);


			// make the opengl texture
			{
				using namespace gl;
				glGenTextures(1, &font->glTextureID);
				glBindTexture(GL_TEXTURE_2D, font->glTextureID);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->atlasWidth, font->atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
					font->atlas);

				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}


		int ascent = 0;
		int descent = 0;
		int linegap = 0;
        stbtt_GetFontVMetrics(&font->fontInfo, &ascent, &descent, &linegap);

        font->ascent = ascent * stbtt_ScaleForPixelHeight(&font->fontInfo, font->pixelSize);
        font->descent = descent * stbtt_ScaleForPixelHeight(&font->fontInfo, font->pixelSize);



		// pre-generate glyph positions
		for(uint32_t ch = firstChar; ch < firstChar + numChars; ch++)
		{
			// font->
		}


		fontMap[tup] = font;
		return font;
	}

	FontGlyphPos Font::getGlyphMetrics(uint32_t character)
	{
		auto it = this->coordCache.find(character);
		if(it != this->coordCache.end())
			return (*it).second;

		float xpos = 0;
		float ypos = 0;

		stbtt_aligned_quad quad;

		stbtt_GetPackedQuad(this->charInfo, this->atlasWidth, this->atlasHeight, character - this->firstChar,
			&xpos, &ypos, &quad, 1);

		auto xmin = quad.x0;
		auto xmax = quad.x1;
		auto ymin = -quad.y1;
		auto ymax = -quad.y0;


		FontGlyphPos ret;

		ret.vertices[0] = glm::vec2(xmin, ymin);
		ret.vertices[1] = glm::vec2(xmax, ymin);
		ret.vertices[2] = glm::vec2(xmax, ymax);
		ret.vertices[3] = glm::vec2(xmin, ymax);

		ret.uvs[0] = glm::vec2(quad.s0, quad.t0);
		ret.uvs[1] = glm::vec2(quad.s1, quad.t0);
		ret.uvs[2] = glm::vec2(quad.s1, quad.t1);
		ret.uvs[3] = glm::vec2(quad.s0, quad.t1);

		auto scale = 0.21875;
		int x0 = 0; int y0 = 0; int x1 = 0; int y1 = 0;
		stbtt_GetCodepointBox(&this->fontInfo, character, &x0, &y0, &x1, &y1);
		fprintf(stderr, "(%c) x0 = %.1f, y0 = %.1f, x1 = %.1f, y1 = %.1f\n", character, x0 * scale, y0 * scale, x1 * scale, y1 * scale);

		ret.xAdvance = this->charInfo->xadvance;
		// ret.height = std::abs(y1 - y0);
		ret.descent = -y0;

		this->coordCache[character] = ret;
		return ret;
	}





	void closeAllFonts()
	{
		for(auto pair : fontMap)
		{
			delete[] pair.second->ttfBuffer;
			delete[] pair.second->atlas;
			delete[] ((stbtt_packedchar*) pair.second->charInfo);
		}
	}
}





