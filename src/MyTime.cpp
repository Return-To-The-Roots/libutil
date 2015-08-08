// $Id: MyTime.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "libUtilDefines.h"
#include "MyTime.h"
#include <ctime>
#include <cstdio>
#include <cstring>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/times.h>
#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p Time.
 *
 *  @author FloSoft
 */
Time::Time(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p Time.
 *
 *  @author FloSoft
 */
Time::~Time(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die aktuelle Zeit.
 *
 *  @return Zeit als Timestamp
 *
 *  @author FloSoft
 */
unser_time_t Time::CurrentTime(void)
{
    return (unser_time_t)time(NULL);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die aktuellen Ticks.
 *
 *  @return Zeit als Ticks
 *
 *  @author FloSoft
 */
unser_time_t Time::CurrentTick(void)
{
    unser_time_t ticks;
#ifdef _WIN32
    ticks = GetTickCount();
#else
    tms tm;
    ticks = times(&tm);
#endif // !_WIN32
    return ticks;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  formatiert einen Zeitstring.
 *
 *  @param[in] ziel   Zielspeicher, muss groß genug sein.
 *  @param[in] format Formatstring (%Y=4 stelliges Jahr, %m 2 stelliges Monat, %D tag, %H Stunde, %i Minute, %s Sekunden)
 *  @param[in] time   zu benutzender Timestamp.
 *
 *  @return @p ziel
 *
 *  @author FloSoft
 */
char* Time::FormatTime(char* ziel, const char* format, unser_time_t* time)
{
    if(!ziel || !format)
        return NULL;

    unser_time_t cur = CurrentTime();

    if(!time)
        time = &cur;

    tm* time_data = localtime((time_t*)time);
    if(!time_data)
        return ziel;

    for(unsigned int i = 0, j = 0; i < strlen(format); ++i)
    {
        bool b = false;
        if(format[i] == '%')
        {
            b = true;
            switch(format[++i])
            {
                case 'Y': j += sprintf(&ziel[j], "%4d", 1900 + time_data->tm_year); break;
                case 'm': j += sprintf(&ziel[j], "%02d", time_data->tm_mon+1); break;
                case 'd': j += sprintf(&ziel[j], "%02d", time_data->tm_mday); break;
                case 'H': j += sprintf(&ziel[j], "%02d", time_data->tm_hour); break;
                case 'i': j += sprintf(&ziel[j], "%02d", time_data->tm_min); break;
                case 's': j += sprintf(&ziel[j], "%02d", time_data->tm_sec); break;
                default: b = false; break;
            }
        }

        if(!b)
        {
            ziel[j] = format[i];
            ziel[++j] = '\0';
        }
    }

    return ziel;
}
