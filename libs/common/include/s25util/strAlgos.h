// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace s25util {
inline char toLower(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}
inline std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](char c) { return toLower(c); });
    return s;
}
inline char toUpper(char ch)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}
inline std::string toUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](char c) { return toUpper(c); });
    return s;
}
} // namespace s25util
