// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MyTime.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/times.h>
#endif // _WIN32
#include "StringConversion.h"

namespace s25util {
/**
 *  liefert die aktuelle Zeit.
 *
 *  @return Zeit als Timestamp
 */
time64_t Time::CurrentTime()
{
    return static_cast<time64_t>(time(nullptr));
}

/**
 *  liefert die aktuellen Ticks.
 *
 *  @return Zeit als Ticks
 */
time64_t Time::CurrentTick()
{
    time64_t ticks;
#ifdef _WIN32
    ticks = GetTickCount();
#else
    tms tm;
    ticks = times(&tm);
#endif // !_WIN32
    return ticks;
}

/**
 *  formatiert einen Zeitstring.
 *
 *  @param[in] format Formatstring (%Y=4 stelliges Jahr, %m 2 stelliges Monat, %D tag, %H Stunde, %i Minute, %s
 * Sekunden)
 *  @param[in] time   zu benutzender Timestamp.
 *
 *  @return Formatted string
 */
std::string Time::FormatTime(const std::string& format, time64_t time)
{
    auto inTime = static_cast<time_t>(time);

    tm* time_data = localtime(&inTime);
    if(!time_data)
        return "";

    using std::setw;
    ClassicImbuedStream<std::stringstream> res;
    res << std::setfill('0');

    bool isInFormat = false;
    for(const auto c : format)
    {
        if(isInFormat)
        {
            isInFormat = false;
            switch(c)
            {
                case 'Y': res << setw(4) << (1900 + time_data->tm_year); break;
                case 'm': res << setw(2) << (time_data->tm_mon + 1); break;
                case 'd': res << setw(2) << time_data->tm_mday; break;
                case 'H': res << setw(2) << time_data->tm_hour; break;
                case 'i': res << setw(2) << time_data->tm_min; break;
                case 's': res << setw(2) << time_data->tm_sec; break;
                case '%': res << '%'; break;
                default:
                    std::cerr << "Invalid format string: " << format << std::endl;
                    res << c;
                    break;
            }
        } else if(c == '%')
            isInFormat = true;
        else
            res << c;
    }

    if(isInFormat)
        std::cerr << "Invalid format string: " << format << std::endl;

    return res.str();
}

std::string Time::FormatTime(const std::string& format)
{
    return FormatTime(format, CurrentTime());
}
} // namespace s25util
