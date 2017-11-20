// Copyright (c) 2016 Settlers Freaks (sf-team at siedler25.org)
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

#include "enumUtils.h"
#include <boost/core/scoped_enum.hpp>
#include <boost/format.hpp>
#include <boost/static_assert.hpp>
#include <string>

class Log;

BOOST_SCOPED_ENUM_DECLARE_BEGIN(LogTarget){Stdout = 1, Stderr = 2, File = 4, FileAndStdout = File | Stdout,
                                           FileAndStderr = File | Stderr} BOOST_SCOPED_ENUM_DECLARE_END(LogTarget)
  MAKE_BITSET_STRONG(LogTarget)

  /// Holds one log entry. Will be flushed on destruction
  class FormatedLogEntry
{
public:
    FormatedLogEntry(Log& log, LogTarget target, const std::string& msg) : log_(log), target_(target), fmt(msg), useColor_(false), color_(0)
    {}
    FormatedLogEntry(Log& log, LogTarget target, const std::string& msg, unsigned color)
        : log_(log), target_(target), fmt(msg), useColor_(true), color_(color)
    {}
    ~FormatedLogEntry();

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
        BOOST_STATIC_ASSERT_MSG(!sizeof(T), "Invalid use of , for messages. Forgot to replace by %?");
    }

private:
    Log& log_;
    LogTarget target_;
    boost::format fmt;
    bool useColor_;
    unsigned color_;
};

#endif // FormatedLogEntry_h__
