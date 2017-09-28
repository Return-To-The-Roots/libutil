// Copyright (c) 2005 - 2016 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <string>

namespace s25Util{

    /// Write the message with fatal-error tag and return EXIT_FAILURE
    int fatal_error(const std::string& msg);

    /// Write the message with error tag and return EXIT_FAILURE
    int error(const std::string& msg);

    /// Write the message with warning tag and return EXIT_SUCCESS
    int warning(const std::string& msg);

}

#endif // !ERROR_H_INCLUDED
