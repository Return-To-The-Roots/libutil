// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <type_traits>

template<typename Enum>
struct IsBitset : std::false_type
{};

template<typename Enum>
constexpr bool IsValidBitset_v = IsBitset<Enum>::value&& std::is_unsigned_v<std::underlying_type_t<Enum>>;

template<typename Enum>
using require_validBitset = std::enable_if_t<IsValidBitset_v<Enum>>;

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
constexpr Enum& operator&=(Enum& lhs, Enum rhs) noexcept
{
    return lhs = lhs & rhs;
}

template<typename Enum, typename = require_validBitset<Enum>>
constexpr Enum& operator|=(Enum& lhs, Enum rhs) noexcept
{
    return lhs = lhs | rhs;
}

namespace bitset {
template<typename Enum, typename = require_validBitset<Enum>>
[[nodiscard]] constexpr Enum clear(const Enum val, const Enum flag)
{
    using Int = std::underlying_type_t<Enum>;
    return val & Enum(~static_cast<const Int>(flag));
}

template<typename Enum, typename = require_validBitset<Enum>>
[[nodiscard]] constexpr Enum set(const Enum val, const Enum flag, const bool state = true)
{
    return state ? (val | flag) : clear(val, flag);
}

template<typename Enum, typename = require_validBitset<Enum>>
[[nodiscard]] constexpr Enum toggle(const Enum val, const Enum flag)
{
    using Int = std::underlying_type_t<Enum>;
    return Enum(static_cast<const Int>(val) ^ static_cast<const Int>(flag));
}

template<typename Enum, typename = require_validBitset<Enum>>
[[nodiscard]] constexpr bool isSet(const Enum val, const Enum flag)
{
    return (val & flag) == flag;
}
} // namespace bitset

/// Makes a strongly typed enum usable as a bitset
#define MAKE_BITSET_STRONG(Type)                                    \
    template<>                                                      \
    struct IsBitset<Type> : std::true_type                          \
    {};                                                             \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                \
    static_assert(std::is_unsigned_v<std::underlying_type_t<Type>>, \
                  #Type " must use unsigned type as the underlying type")
