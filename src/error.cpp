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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "error.h"
#include "Log.h"

namespace s25util {

int fatal_error(const std::string& msg)
{
    LOG.write("\n\nFATAL ERROR: %1%\n") % msg;
    return EXIT_FAILURE;
}

int error(const std::string& msg)
{
    LOG.write("\n\nERROR: %1%\n") % msg;
    return EXIT_FAILURE;
}

int warning(const std::string& msg)
{
    LOG.write("\n\nWARNING: %1%\n") % msg;

    return EXIT_SUCCESS;
}

} // namespace s25util
