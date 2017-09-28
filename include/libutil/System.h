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

#include <boost/filesystem/path.hpp>
#include <string>

/// Class to abstract system specific functions
/// e.g. every #ifdef _WIN32 or similar should be replaced by a function in here
class System
{
public:
    /// Return true iff the environment variable exists
    static bool envVarExists(const std::string& name);
    /// Return true iff the environment variable exists
    static bool envVarExists(const std::wstring& name);
    /// Return the environment variable with the given name
    static std::string getEnvVar(const std::string& name);
    /// Return the environment variable with the given name
    static std::wstring getEnvVar(const std::wstring& name);
    /// Return the environment variable as a path
    /// (might be faster than constructing one from a string)
    static bfs::path getPathFromEnvVar(const std::string& name);

    /// Set the environment variable. Return true on success
    static bool setEnvVar(const std::string& name, const std::string& value);
    /// Set the environment variable. Return true on success
    static bool setEnvVar(const std::wstring& name, const std::wstring& value);
    /// Remove the environment variable. Return true on success
    static bool removeEnvVar(const std::string& name);
    /// Remove the environment variable. Return true on success
    static bool removeEnvVar(const std::wstring& name);

    /// Gets the users home directory
    static bfs::path getHomePath();
    static std::string getUserName();

    /// Return the full path to the current executable
    static bfs::path getExecutablePath(const char* argv0);
    /// Return the full path to the current executable
    static bfs::path getExecutablePath(const std::string& argv0);
};

#endif // System_h__
