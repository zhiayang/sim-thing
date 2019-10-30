// Util.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <random>

#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>

#include "lx.h"
#include "units.h"
#include "profile.h"

#include "stx/string_view.hpp"

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

	template <typename T>
	std::basic_string<T> to_string(const stx::basic_string_view<T>& sv)
	{
		return std::basic_string<T>(sv.data(), sv.size());
	}
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

		colour& operator = (const lx::vec4& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			return *this;
		}

		operator lx::vec4() const { return this->vec4(); }
		operator lx::fvec4() const { return this->fvec4(); }

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

		lx::vec4 vec4() const
		{
			return lx::vec4(this->r, this->g, this->b, this->a);
		}

		lx::fvec4 fvec4() const
		{
			return lx::fvec4(this->r, this->g, this->b, this->a);
		}


		static colour fromHex(uint32_t hex)
		{
			return colour(
				((hex & 0xFF000000) >> 24) / 255.0,
				((hex & 0x00FF0000) >> 16) / 255.0,
				((hex & 0x0000FF00) >>  8) / 255.0,
				((hex & 0x000000FF) >>  0) / 255.0
			);
		}

		static colour fromHexRGB(uint32_t hex)
		{
			return colour(
				((hex & 0xFF0000) >> 16) / 255.0,
				((hex & 0x00FF00) >>  8) / 255.0,
				((hex & 0x0000FF) >>  0) / 255.0
			);
		}

		static colour black()   { return colour(0.0, 0.0, 0.0); }
		static colour white()   { return colour(1.0, 1.0, 1.0); }

		static colour red()     { return colour(1.0, 0.0, 0.0); }
		static colour blue()    { return colour(0.0, 0.0, 1.0); }
		static colour green()   { return colour(0.0, 1.0, 0.0); }
		static colour cyan()    { return colour::green() + colour::blue(); }
		static colour yellow()  { return colour::red() + colour::green(); }
		static colour magenta() { return colour::blue() + colour::red(); }
		static colour random()  { return colour(util::random::get(0, 1), util::random::get(0, 1), util::random::get(0, 1)); }
	};



	template <typename T, class UnaryOp, typename K = typename std::result_of<UnaryOp(T)>::type>
	std::vector<K> map(const std::vector<T>& input, UnaryOp fn)
	{
		std::vector<K> ret;
		for(auto i : input)
			ret.push_back(fn(i));

		return ret;
	}

	template <typename T, class UnaryOp, class Predicate, typename K = typename std::result_of<UnaryOp(T)>::type>
	std::vector<K> filterMap(const std::vector<T>& input, Predicate cond, UnaryOp fn)
	{
		std::vector<K> ret;
		for(auto i : input)
		{
			if(cond(i))
				ret.push_back(fn(i));
		}

		return ret;
	}

	template <typename T, class UnaryOp, class Predicate, typename K = typename std::result_of<UnaryOp(T)>::type>
	std::vector<K> mapFilter(const std::vector<T>& input, UnaryOp fn, Predicate cond)
	{
		std::vector<K> ret;
		for(auto i : input)
		{
			auto k = fn(i);
			if(cond(k)) ret.push_back(k);
		}

		return ret;
	}

	template <typename T, class Predicate>
	std::vector<T> filter(const std::vector<T>& input, Predicate cond)
	{
		std::vector<T> ret;
		for(const auto& i : input)
			if(cond(i))
				ret.push_back(i);

		return ret;
	}

	template <typename T, class Predicate>
	std::vector<T> filterUntil(const std::vector<T>& input, Predicate cond)
	{
		std::vector<T> ret;
		for(const auto& i : input)
		{
			if(cond(i)) ret.push_back(i);
			else        break;
		}

		return ret;
	}

	template <typename T, class Predicate>
	size_t indexOf(const std::vector<T>& input, Predicate cond)
	{
		for(size_t i = 0; i < input.size(); i++)
			if(cond(input[i])) return i;

		return -1;
	}
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






#define S_TO_MS(x)				((x) * 1000.0)
#define S_TO_US(x)				(S_TO_MS(x) * 1000.0)
#define S_TO_NS(x)				(S_TO_US(x) * 1000.0)

#define MS_TO_S(x)				((x) / 1000.0)
#define MS_TO_US(x)				((x) * 1000.0)
#define MS_TO_NS(x)				(MS_TO_US(x) * 1000.0)

#define US_TO_NS(x)				((x) * 1000.0)
#define US_TO_MS(x)				((x) / 1000.0)
#define US_TO_S(x)				(US_TO_MS(x) / 1000.0)

#define NS_TO_US(x)				((x) / 1000.0)
#define NS_TO_MS(x)				(NS_TO_US(x) / 1000.0)
#define NS_TO_S(x)				(NS_TO_MS(x) / 1000.0)
















