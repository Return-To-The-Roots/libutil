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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "fileFuncs.h"
#include "System.h"
#include <boost/filesystem/path.hpp>

/**
 *  formt Pfade korrekt um.
 *
 *  @param[in] file
 *
 *  @return liefert den umgeformten Pfad zurück
 */
std::string GetFilePath(const std::string& file)
{
    if(file.empty())
        return "";

    bfs::path to;

    // ist der Pfad ein Home-Dir?
    if(file[0] == '~')
    {
        to = System::getHomePath();
        if(file.length() > 1)
            to /= file.substr(1);
    } else
        to = file;

    return to.make_preferred().string();
}
