// Font.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <algorithm>

#include <stdio.h>
#include <sys/stat.h>

#include "imgui.h"
#include "graphicswrapper.h"

#include <glbinding/gl/gl.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

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
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->atlasWidth, font->atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
					font->atlas);

				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}


		fontMap[tup] = font;
		return font;
	}

	FontGlyphPos getGlyphPosition(Font* font, uint32_t character)
	{
		float xpos = 0;
		float ypos = 0;

		stbtt_aligned_quad quad;
		memset(&quad, 0, sizeof(stbtt_aligned_quad));

		stbtt_GetPackedQuad((stbtt_packedchar*) font->charInfo, font->atlasWidth, font->atlasHeight, character - font->firstChar,
			&xpos, &ypos, &quad, 1);

		auto xmin = quad.x0;
		auto xmax = quad.x1;
		auto ymin = -quad.y1;
		auto ymax = -quad.y0;

		auto ret = FontGlyphPos();
		// ret.offsetX = offsetX;
		// ret.offsetY = offsetY;

		ret.vertices[0] = glm::vec2(xmin, ymin);
		ret.vertices[1] = glm::vec2(xmax, ymin);
		ret.vertices[2] = glm::vec2(xmax, ymax);
		ret.vertices[3] = glm::vec2(xmin, ymax);

		ret.uvs[0] = glm::vec2(quad.s0, quad.t0);
		ret.uvs[1] = glm::vec2(quad.s1, quad.t0);
		ret.uvs[2] = glm::vec2(quad.s1, quad.t1);
		ret.uvs[3] = glm::vec2(quad.s0, quad.t1);

		// printf("(%f, %f) : (%f, %f) // (%f, %f) : (%f, %f) // (%f, %f)\n",
		// 	xmin, ymin, xmax, ymax, quad.s0, quad.t0, quad.s1, quad.t1, xpos, ypos);

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





