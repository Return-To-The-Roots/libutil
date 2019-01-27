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

#include "fileFuncs.h"
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;

std::string makePortableName(const std::string& fileName)
{
    if(fileName.empty() || bfs::portable_name(fileName))
        return fileName;
    std::string result;
    result.reserve(fileName.size());
    for(char c : fileName)
    {
        // Characters allowed by portable_posix_name. These are also allowed by windows_name
        if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '.' || c == '_' || c == '-')
            result += c;
        else
            result += '_';
    }
    if(result != "." && result != "..")
    {
        // Remove leading and trailing dots
        while(!result.empty() && result.front() == '.')
            result.erase(result.begin());
        while(!result.empty() && result.back() == '.')
            result.erase(result.end() - 1);
    }
    assert(result.empty() || bfs::portable_name(result));
    return result;
}

std::string makePortableFileName(const std::string& fileName)
{
    std::string result = makePortableName(fileName);
    if(result.empty() || bfs::portable_file_name(result))
        return result;
    // Remove all dots but the last
    size_t pos = result.find('.');
    if(pos != std::string::npos)
    {
        size_t nextPos;
        while((nextPos = result.find('.', pos + 1)) != std::string::npos)
        {
            result.erase(pos, 1u);
            pos = nextPos - 1u;
        }
        // Only 3 additional chars after the dot
        if(pos + 4u < result.size())
            result.resize(pos + 4u);
    }
    if(result == "." || result == "..")
        result.clear();
    assert(result.empty() || bfs::portable_file_name(result));
    return result;
}

std::string makePortableDirName(const std::string& fileName)
{
    std::string result = makePortableName(fileName);
    if(result.empty() || bfs::portable_directory_name(result))
        return result;
    // Remove all dots unless we are the special DOT or DOTDOT
    if(result != "." && result != "..")
    {
        size_t pos;
        while((pos = result.find('.')) != std::string::npos)
            result.erase(pos, 1u);
    }
    assert(result.empty() || bfs::portable_directory_name(result));
    return result;
}
