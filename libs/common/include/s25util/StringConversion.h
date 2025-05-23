// Copyright (C) 2005 - 2025 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <climits>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

/// This provides locale independent conversion functions

namespace s25util {
struct ConversionError : public std::runtime_error
{
    ConversionError(const std::string& msg) : std::runtime_error(msg) {}
};

namespace detail {
    template<bool IsFloat>
    struct ToStringClassic;
    void imbueClassic(std::ios& stream);
} // namespace detail

/// A stream that has a classic locale imbued in ctor
template<class T_Base>
struct ClassicImbuedStream : public T_Base
{
    ClassicImbuedStream() { detail::imbueClassic(*this); }
    template<typename T>
    ClassicImbuedStream(const T& value) : T_Base(value)
    {
        detail::imbueClassic(*this);
    }
    template<typename T, typename T2>
    ClassicImbuedStream(const T& value, const T2& value2) : T_Base(value, value2)
    {
        detail::imbueClassic(*this);
    }
};

template<typename T>
inline std::string toStringClassic(const T value, bool toHex = false)
{
    static_assert(std::is_arithmetic_v<T>, "Need an arithmetic value!");
    return detail::ToStringClassic<std::is_floating_point_v<T>>::convert(value, toHex);
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Returns true on success
template<typename T>
inline bool tryFromStringClassic(const std::string& value, T& outVal, bool fromHex = false)
{
    static_assert(std::is_arithmetic_v<T>, "Need an arithmetic value!");
    // Empty = error
    if(value.empty())
        return false;
    // Standard requires underflow when using negative numbers as input to unsigned values. We don't allow this
    if(std::is_unsigned_v<T> && value[0] == '-')
        return false;
    ClassicImbuedStream<std::istringstream> ss(value);
    ss >> std::noskipws;
    if(fromHex)
        ss >> std::hex;
    if(sizeof(T) == sizeof(char))
    {
        // Widen to int for char types to avoid printing it as character
        int tmp;
        if(!(ss >> tmp) || !ss.eof() || tmp < static_cast<int>(std::numeric_limits<T>::min())
           || tmp > static_cast<int>(std::numeric_limits<T>::max()))
            return false;
        outVal = static_cast<T>(tmp);
        return true;
    } else
        return (ss >> outVal) && ss.eof();
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Throws ConversionError on failure.
template<typename T>
inline T fromStringClassic(const std::string& value, bool fromHex = false)
{
    T outVal;
    if(!tryFromStringClassic(value, outVal, fromHex))
        throw ConversionError("Could not convert '" + value + "'");
    return outVal;
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Returns the defaultValue on failure
template<typename T>
inline T fromStringClassicDef(const std::string& value, T defaultValue, bool fromHex = false)
{
    T outVal;
    if(!tryFromStringClassic(value, outVal, fromHex))
        return defaultValue;
    return outVal;
}

namespace detail {
    void enableHexOutput(std::ostream& stream, size_t typeSize);
    template<>
    struct ToStringClassic<false>
    {
        template<typename T>
        static std::string convert(const T value, bool toHex = false)
        {
            static_assert(std::is_integral_v<T>, "Must be integral");
            ClassicImbuedStream<std::ostringstream> ss;
            if(toHex)
                enableHexOutput(ss, sizeof(T));
            // Widen to int for char types to avoid printing it as character
            using U = std::conditional_t<sizeof(T) == sizeof(char), int, T>;
            ss << static_cast<U>(value);
            return ss.str();
        }
    };
    template<>
    struct ToStringClassic<true>
    {
        template<typename T>
        static std::string convert(const T value, bool toHex = false)
        {
            // Calculate required precision as done by Boost.Lexical_Cast
            using limits = std::numeric_limits<T>;
            static_assert(limits::radix == 2 && limits::digits > 0);
            constexpr unsigned long precision = 2UL + limits::digits * 30103UL / 100000UL;
            static_assert(static_cast<unsigned long>(limits::digits) < ULONG_MAX / 30103UL
                          && precision > static_cast<unsigned long>(limits::digits10));

            ClassicImbuedStream<std::ostringstream> ss;
            if(toHex)
                ss << std::hex;
            ss.precision(precision);
            ss << value;
            return ss.str();
        }
    };
} // namespace detail
} // namespace s25util
