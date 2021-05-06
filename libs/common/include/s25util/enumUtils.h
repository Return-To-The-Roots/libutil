// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <type_traits>

/// Makes a strongly typed enum usable as a bitset
#define MAKE_BITSET_STRONG(Type)                                              \
    inline auto operator&(Type lhs, Type rhs)                                 \
    {                                                                         \
        return Type(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)); \
    }                                                                         \
    inline auto operator|(Type lhs, Type rhs)                                 \
    {                                                                         \
        return Type(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs)); \
    }                                                                         \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                          \
    static_assert(std::is_unsigned<std::underlying_type_t<Type>>::value,      \
                  #Type " must use unsigned type as the underlying type")
