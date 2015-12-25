///
/// anax
/// An open source C++ entity system.
///
/// Copyright (C) 2013-2014 Miguel Martin (miguel@miguel-martin.com)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///


#include "anax/detail/BaseSystem.hpp"
#include "anax/detail/AnaxAssert.hpp"

#include <algorithm>

namespace anax
{
	namespace detail
	{
		BaseSystem::BaseSystem(const Filter& filter) : m_world(nullptr), m_filter(filter)
		{
		}

		BaseSystem::~BaseSystem()
		{
		}

		const Filter& BaseSystem::getFilter() const
		{
			return m_filter;
		}

		World& BaseSystem::getWorld() const
		{
			ANAX_ASSERT(m_world != nullptr, "No world attached to system");
			return *m_world;
		}

		const std::vector<Entity>& BaseSystem::getEntities() const
		{
			return m_entities;
		}


		void BaseSystem::add(Entity& entity)
		{
			m_entities.push_back(entity);
			onEntityAdded(entity);
		}

		void BaseSystem::remove(Entity& entity)
		{
			m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
			onEntityRemoved(entity);
		}

		void BaseSystem::setWorld(World& world)
		{
			m_world = &world;
			initialise();
		}
	}
}
