// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#pragma once

#include "Singleton.h"

#ifdef _WIN32
    #if defined __CYGWIN__ || defined __CYGWIN32__ || defined __MINGW__
        #include <sys/types.h>
    #endif
    typedef unsigned __int64 unser_time_t;
#else
    #include <stdint.h>
    typedef int64_t unser_time_t;
#endif

/// Time Klasse.
class Time : public Singleton<Time>
{
    public:
        /// Konstruktor von @p Time.
        Time(void);
        /// Desktruktor von @p Time.
        ~Time(void);

        /// liefert die aktuelle Zeit.
        unser_time_t CurrentTime(void);
        /// liefert die aktuellen Ticks.
        unser_time_t CurrentTick(void);
        /// formatiert einen Zeitstring.
        char* FormatTime(char* ziel, const char* format, unser_time_t* time);
};

#define TIME Time::inst()

#endif // TIME_H_INCLUDED
