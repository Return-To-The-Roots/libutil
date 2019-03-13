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

#include "System.h"
#include "ucString.h"
#include <cstdlib>

namespace bfs = boost::filesystem;

//////////////////////////////////////////////////////////////////////////

bool System::envVarExists(const std::string& name)
{
    return getenv(name.c_str()) != nullptr;
}

bool System::envVarExists(const std::wstring& name)
{
    return envVarExists(cvWideStringToUTF8(name));
}

std::string System::getEnvVar(const std::string& name)
{
    const char* value = getenv(name.c_str());
    return value ? value : "";
}

std::wstring System::getEnvVar(const std::wstring& name)
{
    return cvUTF8ToWideString(getEnvVar(cvWideStringToUTF8(name)));
}

bfs::path System::getPathFromEnvVar(const std::string& name)
{
    return getEnvVar(name);
}

bool System::setEnvVar(const std::string& name, const std::string& value)
{
    return setenv(name.c_str(), value.c_str(), 1) == 0;
}

bool System::setEnvVar(const std::wstring& name, const std::wstring& value)
{
    return setEnvVar(cvWideStringToUTF8(name), cvWideStringToUTF8(value));
}

bool System::removeEnvVar(const std::string& name)
{
    unsetenv(name.c_str());
    return true;
}

bool System::removeEnvVar(const std::wstring& name)
{
    return removeEnvVar(cvWideStringToUTF8(name));
}

bfs::path System::getHomePath()
{
    bfs::path homePath = getPathFromEnvVar("HOME");
    // Emergency fallback. Should never happen but prevents "~/foo"->"/foo"
    if(homePath.empty())
    {
        assert(false);
        homePath = ".";
    }
    return homePath;
}

std::string System::getUserName()
{
    return getEnvVar("USER");
}
