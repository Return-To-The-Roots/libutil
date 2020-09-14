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

#include <cstdint>
#include <string>

namespace s25util {

using time64_t = int64_t;

struct Time
{
public:
    /// liefert die aktuelle Zeit.
    static time64_t CurrentTime();
    /// liefert die aktuellen Ticks.
    static time64_t CurrentTick();
    /// Format a time as a string
    /// @param format Format to use  (%Y=4 year (4 digits), %m month, %D day, %H hour, %i minute, %s second (all 2 digits), %% -> %)
    /// @param time Pointer to time to format or nullptr to use current time
    static std::string FormatTime(const std::string& format, time64_t time);
    /// Format the current time as a string
    static std::string FormatTime(const std::string& format);
};

} // namespace s25util
