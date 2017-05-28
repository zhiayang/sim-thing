// Util.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <random>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>

#include "units.h"
#include "profile.h"

#include <glm/vec4.hpp>

struct ImFont;

namespace stx
{
	template <typename T>
	struct reversion_wrapper { T& iterable; };

	template <typename T>
	auto begin (reversion_wrapper<T> w) { return rbegin(w.iterable); }

	template <typename T>
	auto end (reversion_wrapper<T> w) { return rend(w.iterable); }

	template <typename T>
	reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }
}

namespace util
{
	namespace random
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
		}

		colour& operator = (const colour& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			return *this;
		}

		colour& operator = (const glm::vec4& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			return *this;
		}

		operator glm::vec4() const { return this->toGL(); }

		colour(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) { }
		colour(float red, float green, float blue) : r(red), g(green), b(blue), a(1.0) { }
		colour() : r(0), g(0), b(0), a(0) { }

		float r;
		float g;
		float b;
		float a;

		colour operator+(colour other)
		{
			// stop overflow to zero
			return colour(std::max(this->r + other.r, 1.0f), std::max(this->g + other.g, 1.0f),
				std::max(this->b + other.b, 1.0f), std::max(this->a + other.a, 1.0f));
		}

		glm::vec4 toGL() const
		{
			return glm::vec4(this->r, this->g, this->b, this->a);
		}

		static colour fromHex(uint32_t hex)
		{
			return colour((hex & 0xFF000000 >> 24) / 255.0, (hex & 0x00FF0000 >> 16) / 255.0,
				(hex & 0x0000FF00 >> 8) / 255.0, (hex & 0xFF) / 255.0);
		}

		static colour black() { return colour(0, 0, 0); }
		static colour white() { return colour(1.0, 1.0, 1.0); }

		static colour red() { return colour(1.0, 0, 0); }
		static colour blue() { return colour(0, 0, 1.0); }
		static colour green() { return colour(0, 1.0, 0); }
		static colour cyan() { return colour::green() + colour::blue(); }
		static colour yellow() { return colour::red() + colour::green(); }
		static colour magenta() { return colour::blue() + colour::red(); }
		static colour random() { return colour(util::random::get(0, 1), util::random::get(0, 1), util::random::get(0, 1)); }
	};
}

namespace Logging
{
	struct Logger
	{
		void Error(const char* fmt, ...) __attribute__((noreturn));
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


















