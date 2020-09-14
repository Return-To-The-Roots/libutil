// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include <string>

/// Remove all invalid chars of a file or directory name. Result may be empty!
/// --> bfs::portable_name will return true
std::string makePortableName(const std::string& fileName);
/// Remove all invalid chars so the name can be used for a file. Result may be empty!
/// --> bfs::portable_file_name will return true
std::string makePortableFileName(const std::string& fileName);
/// Remove all invalid chars so the name can be used for a directory. Result may be empty!
/// --> bfs::portable_directory_name will return true
std::string makePortableDirName(const std::string& fileName);
