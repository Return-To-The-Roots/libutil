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
#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#pragma once

#include "Singleton.h"
#include <cstdarg>
#include <cstdio>

/// Log Klasse.
class Log : public Singleton<Log, SingletonPolicies::WithLongevity>
{
    public:
        static const unsigned Longevity = 100;

        /// Konstruktor von @p Log.
        Log();
        /// Desktruktor von @p Log.
        ~Log() override;

        /// Öffnet das Log (falls noch nicht offen)
        void open();

        void getlasterror(const char* text);

        /// Schreibt gefärbte Daten ins Log und auf stdout.
        void lcprintf(const unsigned int color, const char* format, ...);
        /// Schreibt Daten ins Log und auf stdout.
        void lprintf(const char* format, ...);
        /// Schreibt Daten ins Log und auf stdout.
        void lvprintf(const char* format, va_list list);

        /// Schreibt Daten nur ins Log und auf stdout.
        void write(const char* format, ...);
        /// Schreibt Daten nur ins Log und auf stdout.
        void vwrite(const char* format, va_list list);

    private:
        FILE* log; /// das Logfile
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
