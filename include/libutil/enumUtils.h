// Copyright (c) 2016 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once
#ifndef enumUtils_h__
#define enumUtils_h__

#include <boost/core/scoped_enum.hpp>

#ifdef BOOST_NO_CXX11_SCOPED_ENUMS
#define MAKE_BITSET_STRONG_UL(Type)                                                   \
    \
inline Type                                                                           \
    operator&(Type lhs, BOOST_SCOPED_ENUM_NATIVE(Type) rhs)                           \
    {                                                                                 \
        return Type(boost::underlying_cast<int>(lhs) & rhs);                          \
    }                                                                                 \
    \
inline Type                                                                           \
    operator&(BOOST_SCOPED_ENUM_NATIVE(Type) lhs, Type rhs)                           \
    {                                                                                 \
        return Type(lhs & boost::underlying_cast<int>(rhs));                          \
    }                                                                                 \
    \
inline Type                                                                           \
    operator&(BOOST_SCOPED_ENUM_NATIVE(Type) lhs, BOOST_SCOPED_ENUM_NATIVE(Type) rhs) \
    {                                                                                 \
        return Type(lhs & rhs);                                                       \
    }                                                                                 \
    \
inline Type                                                                           \
    operator|(Type lhs, BOOST_SCOPED_ENUM_NATIVE(Type) rhs)                           \
    {                                                                                 \
        return Type(boost::underlying_cast<int>(lhs) | rhs);                          \
    }                                                                                 \
    \
inline Type                                                                           \
    operator|(BOOST_SCOPED_ENUM_NATIVE(Type) lhs, Type rhs)                           \
    {                                                                                 \
        return Type(lhs | boost::underlying_cast<int>(rhs));                          \
    }                                                                                 \
    \
inline Type                                                                           \
    operator|(BOOST_SCOPED_ENUM_NATIVE(Type) lhs, BOOST_SCOPED_ENUM_NATIVE(Type) rhs) \
    {                                                                                 \
        return Type(lhs | rhs);                                                       \
    }
#else
#define MAKE_BITSET_STRONG_UL(Type)
#endif

/// Makes a strongly typed enum usable as a bitset
#define MAKE_BITSET_STRONG(Type)                                                          \
    \
inline Type                                                                               \
    operator&(Type lhs, Type rhs)                                                         \
    {                                                                                     \
        return Type(boost::underlying_cast<int>(lhs) & boost::underlying_cast<int>(rhs)); \
    }                                                                                     \
    \
inline Type                                                                               \
    operator|(Type lhs, Type rhs)                                                         \
    {                                                                                     \
        return Type(boost::underlying_cast<int>(lhs) | boost::underlying_cast<int>(rhs)); \
    }                                                                                     \
    \
MAKE_BITSET_STRONG_UL(Type)

#endif // enumUtils_h__
