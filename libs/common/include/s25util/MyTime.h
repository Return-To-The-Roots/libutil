// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
    /// @param format Format to use  (%Y=4 year (4 digits), %m month, %D day, %H hour, %i minute, %s second (all 2
    /// digits), %% -> %)
    /// @param time Pointer to time to format or nullptr to use current time
    static std::string FormatTime(const std::string& format, time64_t time);
    /// Format the current time as a string
    static std::string FormatTime(const std::string& format);
};

} // namespace s25util
