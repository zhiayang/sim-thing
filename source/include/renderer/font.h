// font.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <unordered_map>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

#include <glm/mat4x4.hpp>

#include "stb_truetype.h"

namespace Rx
{
	// font stuff
	struct FontGlyphPos
	{
		glm::vec2 vertices[4];
		glm::vec2 uvs[4];

		double xAdvance = 0;

		double descent = 0;
	};

	struct Font
	{
		Font() { }
		Font(std::string n) : name(n) { }

		Font(const Font&) = delete;
		Font& operator = (const Font&) = delete;

		std::string name;

		size_t pixelSize = 0;

		uint8_t* ttfBuffer = 0;
		size_t ttfBufferSize = 0;

		uint8_t* atlas = 0;
		size_t atlasWidth = 0;
		size_t atlasHeight = 0;

		size_t vertOversample = 0;
		size_t horzOversample = 0;

		uint32_t firstChar = 0;
		size_t numChars = 0;

		gl::GLuint glTextureID = -1;

		stbtt_fontinfo fontInfo;
		stbtt_packedchar* charInfo = 0;

		double descent = 0;
		double ascent = 0;

		std::unordered_map<uint32_t, FontGlyphPos> coordCache;

		FontGlyphPos getGlyphMetrics(uint32_t u32);
	};

	Font* getFont(std::string name, size_t pixelSize, uint32_t firstChar, size_t numChars, size_t oversampleH, size_t oversampleV);
	void closeAllFonts();
}










