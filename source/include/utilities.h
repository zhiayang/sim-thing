// Util.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <random>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>

#include "units.h"

#include <glm/vec4.hpp>

struct ImFont;

namespace util
{
	namespace Random
	{
		double get(double scale = 1);
		double get(double lower, double upper);
		void cycle();
	};

	namespace Time
	{
		inline uint64_t ns()
		{
			using namespace std;
			return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
		}

		inline double µs()
		{
			return ns() / 1000.0;
		}

		inline double ms()
		{
			return µs() / 1000.0;
		}

		inline double sec()
		{
			return ms() / 1000.0;
		}
	};

	struct colour
	{
		colour(const colour& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			this->fr = other.fr;
			this->fg = other.fg;
			this->fb = other.fb;
			this->fa = other.fa;
		}

		colour& operator = (const colour& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			this->fr = other.fr;
			this->fg = other.fg;
			this->fb = other.fb;
			this->fa = other.fa;

			return *this;
		}

		colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : r(red), g(green), b(blue), a(alpha)
		{
			fr = (1.0f / 255.0f) * r;
			fg = (1.0f / 255.0f) * g;
			fb = (1.0f / 255.0f) * b;
			fa = (1.0f / 255.0f) * a;
		}

		colour(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue), a(0xFF)
		{
			fr = (1.0f / 255.0f) * r;
			fg = (1.0f / 255.0f) * g;
			fb = (1.0f / 255.0f) * b;
			fa = (1.0f / 255.0f) * a;
		}

		colour() : r(0), g(0), b(0), a(0)
		{
			fr = (1.0f / 255.0f) * r;
			fg = (1.0f / 255.0f) * g;
			fb = (1.0f / 255.0f) * b;
			fa = (1.0f / 255.0f) * a;
		}

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		float fr;
		float fg;
		float fb;
		float fa;

		colour operator+(colour other)
		{
			// stop overflow to zero
			return colour(std::max(this->r + other.r, 0xFF), std::max(this->g + other.g, 0xFF),
				std::max(this->b + other.b, 0xFF), std::max(this->a + other.a, 0xFF));
		}

		uint32_t hex()
		{
			return (r << 24) | (g << 16) | (b << 8) | a;
		}

		glm::vec4 toVec4()
		{
			return glm::vec4(this->fr, this->fg, this->fb, this->fa);
		}

		static colour black() { return colour(0, 0, 0); }
		static colour white() { return colour(0xFF, 0xFF, 0xFF); }

		static colour red() { return colour(0xFF, 0, 0); }
		static colour blue() { return colour(0, 0, 0xFF); }
		static colour green() { return colour(0, 0xFF, 0); }
		static colour cyan() { return colour::green() + colour::blue(); }
		static colour yellow() { return colour::red() + colour::green(); }
		static colour magenta() { return colour::blue() + colour::red(); }
		static colour random() { return colour((uint8_t) util::Random::get(0, 255), (uint8_t) util::Random::get(0, 255), (uint8_t) util::Random::get(0, 255)); }
	};
}

namespace Logging
{
	struct Logger
	{
		void Error(const char* fmt, ...);
		void Info(const char* fmt, ...);
		void Warn(const char* fmt, ...);
	};

	std::shared_ptr<Logger> GetGlobalLoggerInstance();
}

#define LOG(fmt, ...)			Logging::GetGlobalLoggerInstance()->Info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)			Logging::GetGlobalLoggerInstance()->Warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...)			Logging::GetGlobalLoggerInstance()->Error(fmt, ##__VA_ARGS__)







#define NS_TO_US(x)				((x) / 1000)
#define NS_TO_MS(x)				(NS_TO_US(x) / 1000)
#define NS_TO_S(x)				(NS_TO_MS(x) / 1000)

#define S_TO_MS(x)				((x) * 1000)
#define S_TO_US(x)				(S_TO_MS(x) * 1000)
#define S_TO_NS(x)				(S_TO_US(x) * 1000)


















