// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <type_traits>

template<typename Enum>
struct IsBitset : std::false_type
{};

template<typename Enum>
constexpr bool IsValidBitset = IsBitset<Enum>::value && std::is_unsigned<std::underlying_type_t<Enum>>::value;

template<typename Enum>
using require_validBitset = std::enable_if_t<IsValidBitset<Enum>>;

template<typename Enum, typename = require_validBitset<Enum>>
constexpr auto operator&(Enum lhs, Enum rhs) noexcept
{
    using Int = std::underlying_type_t<Enum>;
    return Enum(static_cast<Int>(lhs) & static_cast<Int>(rhs));
}

template<typename Enum, typename = require_validBitset<Enum>>
constexpr auto operator|(Enum lhs, Enum rhs) noexcept
{
    using Int = std::underlying_type_t<Enum>;
    return Enum(static_cast<Int>(lhs) | static_cast<Int>(rhs));
}

template<typename Enum, typename = require_validBitset<Enum>>
constexpr auto operator&=(Enum& lhs, Enum rhs) noexcept
{
    return lhs = lhs & rhs;
}

template<typename Enum, typename = require_validBitset<Enum>>
constexpr auto operator|=(Enum& lhs, Enum rhs) noexcept
{
    return lhs = lhs | rhs;
}

/// Makes a strongly typed enum usable as a bitset
#define MAKE_BITSET_STRONG(Type)                                         \
    template<>                                                           \
    struct IsBitset<Type> : std::true_type                               \
    {};                                                                  \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                     \
    static_assert(std::is_unsigned<std::underlying_type_t<Type>>::value, \
                  #Type " must use unsigned type as the underlying type")
