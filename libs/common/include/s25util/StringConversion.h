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

#pragma once

#ifndef StringConversion_h__
#define StringConversion_h__

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
    template<typename T, typename = void>
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
inline std::string toStringClassic(const T value)
{
    static_assert(std::is_arithmetic<T>::value, "Need an arithmetic value!");
    return detail::ToStringClassic<T>::convert(value);
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Returns true on success
template<typename T>
inline bool tryFromStringClassic(const std::string& value, T& outVal)
{
    static_assert(std::is_arithmetic<T>::value, "Need an arithmetic value!");
    // Empty = error
    if(value.empty())
        return false;
    // Standard requires underflow when using negative numbers as input to unsigned values. We don't allow this
    if(std::is_unsigned<T>::value && value[0] == '-')
        return false;
    ClassicImbuedStream<std::istringstream> ss(value);
    ss >> std::noskipws >> outVal;
    return !!ss && ss.eof();
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Throws ConversionError on failure.
template<typename T>
inline T fromStringClassic(const std::string& value)
{
    T outVal;
    if(!tryFromStringClassic(value, outVal))
        throw ConversionError("Could not convert " + value);
    return outVal;
}

/// Tries to convert from source to target type using either a static_cast or a locale independent string conversion
/// Returns the defaultValue on failure
template<typename T>
inline T fromStringClassicDef(const std::string& value, T defaultValue)
{
    T outVal;
    if(!tryFromStringClassic(value, outVal))
        return defaultValue;
    return outVal;
}

namespace detail {
    template<typename T>
    struct ToStringClassic<T, std::enable_if_t<std::is_integral<T>::value>>
    {
        static std::string convert(const T value)
        {
            ClassicImbuedStream<std::ostringstream> ss;
            ss << std::noskipws << value;
            return ss.str();
        }
    };
    template<typename T>
    struct ToStringClassic<T, std::enable_if_t<std::is_floating_point<T>::value>>
    {
        // Calculate required precision as done by Boost.Lexical_Cast
        using limits = std::numeric_limits<T>;
        static_assert(limits::radix == 2 && limits::digits > 0, "");
        static constexpr unsigned long precision = 2UL + limits::digits * 30103UL / 100000UL;
        static_assert(static_cast<unsigned long>(limits::digits) < ULONG_MAX / 30103UL
                        && precision > static_cast<unsigned long>(limits::digits10),
                      "");

        static std::string convert(const T value)
        {
            ClassicImbuedStream<std::ostringstream> ss;
            ss.precision(precision);
            ss << value;
            return ss.str();
        }
    };
} // namespace detail
} // namespace s25util

#endif // StringConversion_h__
