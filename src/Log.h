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

#pragma once

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include "Singleton.h"
#include <cstdarg>
#include <cstdio>

class Log : public Singleton<Log, SingletonPolicies::WithLongevity>
{
    public:
        static const unsigned Longevity = 100;

        Log();
        ~Log() override;

        /// Opens the log file if it is not yet open
        void open();
        /// Writes the last occured error description with "[text]: " at the front to stdOut and file
        void writeLastError(const char* text);

        /// Writes colored text to stdOut and file
        void writeColored(const unsigned int color, const char* format, ...);
        /// Writes formated text to stdOut and file
        void write(const char* format, ...);
        /// Writes formated text to stdOut and file using a va_list
        void writeVArgs(const char* format, va_list list);

        /// Writes formated text to file only
        void writeToFile(const char* format, ...);
        /// Writes formated text to file only using a va_list
        void writeToFileVArgs(const char* format, va_list list);

    private:

        FILE* logFile;
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
