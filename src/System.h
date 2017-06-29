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

#pragma once

#ifndef System_h__
#define System_h__

#include <string>
#include <boost/filesystem/path.hpp>

/// Class to abstract system specific functions
/// e.g. every #ifdef _WIN32 or similar should be replaced by a function in here
class System
{
public:
    /// Return the environment variable with the given name
    static std::string getEnvVar(const std::string& name);
    /// Return the environment variable with the given name
    static std::wstring getEnvVar(const std::wstring& name);
    /// Return the environment variable as a path
    /// (might be faster than constructing one from a string)
    static bfs::path getPathFromEnvVar(const std::string& name);
    /// Return true iff the environment variable exists
    static bool envVarExists(const std::string& name);
    /// Gets the users home directory
    static bfs::path getHomePath();
    static std::string getUserName();
};


#endif // System_h__
