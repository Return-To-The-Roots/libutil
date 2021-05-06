// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <string>

/// Class to abstract system specific functions
/// e.g. every #ifdef _WIN32 or similar should be replaced by a function in here
class System
{
public:
    /// Changes the current directory and back when destroyed
    /// Note: Failure in initial change throws and in final change terminates the program
    class ScopedCurrentPathChange
    {
        const boost::filesystem::path oldCurPath;

    public:
        explicit ScopedCurrentPathChange(const boost::filesystem::path& newCurrentPath);
        ~ScopedCurrentPathChange();
    };

    /// Return true iff the environment variable exists
    static bool envVarExists(const std::string& name);
    /// Return the environment variable with the given name
    static std::string getEnvVar(const std::string& name);
    /// Return the environment variable as a path
    /// (might be faster than constructing one from a string)
    static boost::filesystem::path getPathFromEnvVar(const std::string& name);

    /// Set the environment variable. Return true on success
    static bool setEnvVar(const std::string& name, const std::string& value);
    /// Remove the environment variable. Return true on success
    static bool removeEnvVar(const std::string& name);

    /// Gets the users home directory
    static boost::filesystem::path getHomePath();
    static std::string getUserName();

    /// Return the full path to the current executable
    static boost::filesystem::path getExecutablePath();
    /// Return the name of the OS
    static std::string getOSName();
    /// Get the name of the compiler
    static std::string getCompilerName();
    /// Execute the given command with the given arguments
    /// The executable must not contain a space (the path to it may)
    /// Arguments need to be properly quoted
    static bool execute(const boost::filesystem::path& command, const std::string& arguments = "");
};
