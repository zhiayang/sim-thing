// Font.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <map>
#include <algorithm>

#include <stdio.h>
#include <sys/stat.h>

#include "rx.h"

#include <glbinding/gl/gl.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_pack_rect.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

namespace rx
{
	typedef std::pair<std::string, size_t> FontTuple;
	static std::map<FontTuple, Font*> fontMap;

	Font* getFont(std::string name, size_t pixelSize, uint32_t firstChar, size_t numChars, size_t oversampleH, size_t oversampleV)
	{
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

		{
			size_t atlasWidth = numperside * maxpixsize;
			size_t atlasHeight = numperside * maxpixsize;
			uint8_t* atlasBuffer = new uint8_t[atlasWidth * atlasHeight];
			assert(atlasBuffer && "failed to allocate memory for atlas");
			memset(atlasBuffer, 0, atlasWidth * atlasHeight);

			// autotex = false -- we want to generate the texture ourselves.
			font->fontAtlas = new Texture(atlasBuffer, atlasWidth, atlasHeight, ImageFormat::GREYSCALE, false);
		}

		LOG("Allocated a %s font atlas (%zux%zu) for font '%s' at size %zupx",
			Units::formatWithUnits(font->fontAtlas->width * font->fontAtlas->height, 1, "B").c_str(),
				font->fontAtlas->width, font->fontAtlas->height, name.c_str(), pixelSize);


		stbtt_InitFont(&font->fontInfo, font->ttfBuffer, 0);


		// begin stb_truetype stuff
		{
			stbtt_pack_context context;
			if(!stbtt_PackBegin(&context, font->fontAtlas->surf->data, font->fontAtlas->width, font->fontAtlas->height, 0, 1, nullptr))
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

				glGenTextures(1, &font->fontAtlas->glTextureID);
				glBindTexture(GL_TEXTURE_2D, font->fontAtlas->glTextureID);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				// minification is more important, since we usually oversample and down-scale for better-looking text
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				float maxAnisotropicFiltering = 0;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropicFiltering);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropicFiltering);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->fontAtlas->width, font->fontAtlas->height, 0, GL_RED, GL_UNSIGNED_BYTE,
					font->fontAtlas->surf->data);

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






		// make some VBOs that contain all the vertex and UV coordinates
		{
			using namespace gl;

			font->renderObject = new RenderObject();
			glBindVertexArray(font->renderObject->vertexArrayObject);

			GLuint uvBuffer = 0; glGenBuffers(1, &uvBuffer);
			GLuint vertBuffer = 0; glGenBuffers(1, &vertBuffer);

			font->renderObject->buffers = { uvBuffer, vertBuffer };

			std::vector<lx::fvec2> verts;
			std::vector<lx::fvec2> uvs;

			// make the VBO.
			for(uint32_t ch = firstChar; ch < firstChar + numChars; ch++)
			{
				auto fgp = font->getGlyphMetrics(ch);

				verts.push_back(lx::round(lx::fvec2(fgp.x1, fgp.y0)));	// 3
				verts.push_back(lx::round(lx::fvec2(fgp.x0, fgp.y1)));	// 2
				verts.push_back(lx::round(lx::fvec2(fgp.x0, fgp.y0)));	// 1

				verts.push_back(lx::round(lx::fvec2(fgp.x0, fgp.y1)));
				verts.push_back(lx::round(lx::fvec2(fgp.x1, fgp.y0)));
				verts.push_back(lx::round(lx::fvec2(fgp.x1, fgp.y1)));

				uvs.push_back(lx::fvec2(fgp.u1, fgp.v0));	// 3
				uvs.push_back(lx::fvec2(fgp.u0, fgp.v1));	// 2
				uvs.push_back(lx::fvec2(fgp.u0, fgp.v0));	// 1

				uvs.push_back(lx::fvec2(fgp.u0, fgp.v1));
				uvs.push_back(lx::fvec2(fgp.u1, fgp.v0));
				uvs.push_back(lx::fvec2(fgp.u1, fgp.v1));
			}

			glEnableVertexAttribArray(0);
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
				glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(lx::fvec2), &verts[0], GL_STATIC_DRAW);

				glVertexAttribPointer(
					0,			// location
					2,			// size
					GL_FLOAT,	// type
					GL_FALSE,	// normalized?
					0,			// stride
					(void*) 0	// array buffer offset
				);
			}

			glEnableVertexAttribArray(1);
			{
				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
				glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(lx::fvec2), &uvs[0], GL_STATIC_DRAW);

				glVertexAttribPointer(
					1,			// location
					2,			// size
					GL_FLOAT,	// type
					GL_FALSE,	// normalized?
					0,			// stride
					(void*) 0	// array buffer offset
				);
			}

			font->renderObject->arrayLength = verts.size();
			font->renderObject->renderType = RenderType::Text;
			font->renderObject->material = Material(util::colour::white(), font->fontAtlas, font->fontAtlas, 1);


			glBindVertexArray(0);
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
//
		stbtt_aligned_quad quad;

		stbtt_GetPackedQuad(this->charInfo, this->fontAtlas->width, this->fontAtlas->height, character - this->firstChar,
			&xpos, &ypos, &quad, 0);

		auto x0 = quad.x0;
		auto x1 = quad.x1;
		auto y0 = quad.y1;
		auto y1 = quad.y0;

		FontGlyphPos ret;

		ret.x0 = x0;
		ret.y0 = y0;
		ret.x1 = x1;
		ret.y1 = y1;

		// note that we flip the vertical tex coords, since opengl works with (0, 0) at bottom-left
		// but stb_truetype gives us (0, 0) at top-left.
		ret.u0 = quad.s0;
		ret.v0 = quad.t1;
		ret.u1 = quad.s1;
		ret.v1 = quad.t0;

		// fprintf(stderr, "glyph ('%c'):\nxy0: (%f, %f)\nxy1: (%f, %f)\n\n", character, x0, y0, x1, y1);

		ret.xAdvance = this->charInfo->xadvance;
		ret.descent = -y0;

		this->coordCache[character] = ret;
		return ret;
	}





	void closeAllFonts()
	{
		for(auto pair : fontMap)
		{
			delete[] pair.second->ttfBuffer;
			delete[] pair.second->fontAtlas;
			delete[] ((stbtt_packedchar*) pair.second->charInfo);
		}
	}
}





