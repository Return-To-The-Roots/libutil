// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef Deleter_h__
#define Deleter_h__

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/remove_extent.hpp>
#include <boost/utility/enable_if.hpp>

template<typename T>
struct Deleter
{
    template<typename U>
    struct can_assign
    {
        BOOST_STATIC_CONSTEXPR bool value =
          boost::is_array<T>::value == boost::is_array<U>::value
          && boost::is_convertible<typename boost::remove_extent<U>::type*, typename boost::remove_extent<T>::type*>::value;
    };
    template<class U>
    void operator()(U* ptr) const
    {
        typedef typename boost::remove_extent<T>::type Type;
        // complete type
        BOOST_STATIC_ASSERT(sizeof(U) > 0);
        Type* p = static_cast<Type*>(ptr);
        boost::is_array<T>::value ? delete[] p : delete p;
    }

    Deleter() {}
    Deleter(const Deleter&) {}
    template<class U>
    Deleter(const Deleter<U>&, typename boost::enable_if<can_assign<U> >::type* = NULL)
    {}
    template<class U>
    typename boost::enable_if<can_assign<U>, Deleter&>::type operator=(const Deleter<U>&)
    {
        return *this;
    }
};

#endif // Deleter_h__
