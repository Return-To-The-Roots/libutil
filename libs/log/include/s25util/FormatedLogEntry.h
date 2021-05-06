// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "s25util/enumUtils.h"
#include <boost/format.hpp>

class Log;

enum class LogTarget : unsigned
{
    Stdout = 1,
    Stderr = 2,
    File = 4,
    StdoutAndStderr = Stdout | Stderr,
    FileAndStdout = File | Stdout,
    FileAndStderr = File | Stderr,
    All = Stdout | Stderr | File
};
MAKE_BITSET_STRONG(LogTarget);

/// Holds one log entry. Will be flushed on destruction
class FormatedLogEntry
{
public:
    FormatedLogEntry(Log& log, LogTarget target, const char* msg, unsigned color = 0u)
        : log_(log), target_(target), fmt(msg), color_(color)
    {}
    FormatedLogEntry(FormatedLogEntry&& rhs) noexcept : log_(rhs.log_), target_(rhs.target_), color_(rhs.color_)
    {
        fmt.swap(rhs.fmt);
    }
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
