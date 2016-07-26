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

#include <boost/format.hpp>
#include <string>

class Log;

/// Holds one log entry. Will be flushed on destruction
class FormatedLogEntry {
public:
    enum LogTarget{
        TO_FILE,
        TO_FILE_AND_STDOUT
    };
    FormatedLogEntry(Log& log, LogTarget target, const char* msg): log_(log), target_(target), fmt(msg) {}
    FormatedLogEntry(Log& log, LogTarget target, const std::string& msg): log_(log), target_(target), fmt(msg) {}
    ~FormatedLogEntry();

    template<typename T>
    FormatedLogEntry& operator%(T value) {
        fmt % value;
        return *this;
    }

private:
    Log& log_;
    LogTarget target_;
    boost::format fmt;
};

#endif // FormatedLogEntry_h__
