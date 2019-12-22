// Copyright (c) 2016 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include <type_traits>

/// Makes a strongly typed enum usable as a bitset
#define MAKE_BITSET_STRONG(Type)                                                                                        \
    inline auto operator&(Type lhs, Type rhs) { return Type(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)); } \
    inline auto operator|(Type lhs, Type rhs) { return Type(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs)); } \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                                                    \
    static_assert(std::is_unsigned<std::underlying_type_t<Type>>::value, #Type " must use unsigned type as the underlying type")

#endif // enumUtils_h__
