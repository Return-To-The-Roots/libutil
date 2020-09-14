// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
