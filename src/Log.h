// $Id: Log.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

/// Log Klasse.
class Log : public Singleton<Log>
{
    public:
        /// Konstruktor von @p Log.
        Log(void);
        /// Desktruktor von @p Log.
        ~Log(void);

        /// öffnet das Log (falls noch nicht offen)
        void open(void);

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
        FILE* log; ///< das Logfile
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
