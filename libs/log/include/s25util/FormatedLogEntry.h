// Copyright (c) 2016 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef FormatedLogEntry_h__
#define FormatedLogEntry_h__

#include "s25util/enumUtils.h"
#include <boost/format.hpp>

class Log;

enum class LogTarget
{
    Stdout = 1,
    Stderr = 2,
    File = 4,
    StdoutAndStderr = Stdout | Stderr,
    FileAndStdout = File | Stdout,
    FileAndStderr = File | Stderr,
    All = Stdout | Stderr | File
};
MAKE_BITSET_STRONG(LogTarget)

/// Holds one log entry. Will be flushed on destruction
class FormatedLogEntry
{
public:
    FormatedLogEntry(Log& log, LogTarget target, const char* msg, unsigned color = 0u) : log_(log), target_(target), fmt(msg), color_(color)
    {}
    FormatedLogEntry(FormatedLogEntry&& rhs) noexcept : log_(rhs.log_), target_(rhs.target_), color_(rhs.color_) { fmt.swap(rhs.fmt); }
    FormatedLogEntry(const FormatedLogEntry&) = delete;
    FormatedLogEntry& operator=(const FormatedLogEntry&) = delete;
    // The destructor constructs the final message from the format and flushs it to the log
    // This may throw in either operation which is ok, as this is only intended to be used as a temporary
    // NOLINTNEXTLINE(bugprone-exception-escape)
    ~FormatedLogEntry() noexcept(false);

    template<typename T>
    FormatedLogEntry& operator%(const T& value)
    {
        fmt % value;
        return *this;
    }

    // comma operator designed to fail to avoid accidental use
    template<typename T>
    void operator,(T)
    {
        static_assert(!sizeof(T), "Invalid use of , for messages. Forgot to replace by %?");
    }

private:
    Log& log_;
    LogTarget target_;
    boost::format fmt;
    unsigned color_;
};

#endif // FormatedLogEntry_h__
