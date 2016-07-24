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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "StringStreamWriter.h"
#include <stdexcept>
#include <cstdio>

void StringStreamWriter::writeFormattedText(const char* format, va_list list)
{
    char buffer[4096];
#if defined(_MSC_VER) || __cplusplus >= 201103L
    // Use safer C++11 function
    int written = vsnprintf(buffer, sizeof(buffer), format, list);
    if(written < 0 || written >= sizeof(buffer))
        throw std::runtime_error("Buffer overflow!");
#else
    // Some kind of protection by limitting format size
    if(strlen(format) > sizeof(buffer) / 2)
        throw std::runtime_error("Possble buffer overflow!");
    int written = vsprintf(buffer, format, list);
    if(written < 0)
        throw std::runtime_error("Error on logging!");
    else if(written >= sizeof(buffer))
        throw std::runtime_error("Buffer overflow!");
#endif
    stream << buffer;
}

std::string StringStreamWriter::getText() const
{
    return stream.str();
}
