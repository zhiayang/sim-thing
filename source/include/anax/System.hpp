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

#ifndef ANAX_SYSTEM_HPP
#define ANAX_SYSTEM_HPP

#include <cstddef>
#include <vector>

#include "anax/FilterOptions.hpp"

#include "anax/detail/BaseSystem.hpp"
#include "anax/detail/ClassTypeId.hpp"

namespace anax
{
	/// \brief A class that follows the CRTP pattern, used to define custom systems
	/// \\tparam T The System you are defining
	/// \tparam RequireList stuff
	///
	/// This class uses the CRTP pattern to make a unique identifier for each system
	/// class
	///
	/// \see BaseSystem
	/// If you wish to store systems generically and for further documentation.
	///
	/// \author Miguel Martin
	template <class RequireList, class ExcludeList = Excludes<>>
	class System : public detail::BaseSystem
	{
	public:

		/// Default constructor
		System() : BaseSystem { detail::MakeFilter<RequireList, ExcludeList>() }
		{
		}
	};

	template<class T>
	detail::TypeId SystemTypeId()
	{
		return detail::ClassTypeId<detail::BaseSystem>::GetTypeId<T>();
	}
}

#endif // ANAX_SYSTEM_HPP
