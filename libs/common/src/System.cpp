// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "System.h"
#include "getExecutablePath.h"
#include <boost/filesystem/operations.hpp>
#include <boost/nowide/cstdlib.hpp>

namespace bfs = boost::filesystem;
namespace bnw = boost::nowide;

bool System::envVarExists(const std::string& name)
{
    return bnw::getenv(name.c_str()) != nullptr;
}

std::string System::getEnvVar(const std::string& name)
{
    const char* value = bnw::getenv(name.c_str());
    return value ? value : "";
}

bfs::path System::getPathFromEnvVar(const std::string& name)
{
    return getEnvVar(name);
}

bool System::setEnvVar(const std::string& name, const std::string& value)
{
    return bnw::setenv(name.c_str(), value.c_str(), 1) == 0;
}

bool System::removeEnvVar(const std::string& name)
{
    bnw::unsetenv(name.c_str());
    return true;
}

bfs::path System::getExecutablePath()
{
    return ::getExecutablePath();
}

std::string System::getOSName()
{
    std::string name = BOOST_PLATFORM;
    if(name.empty())
        name = "Unknown OS";
    if(sizeof(void*) == 4)
        name += " 32";
    else if(sizeof(void*) == 8)
        name += " 64";
    else
        name += " unknown";
    name += " Bit";
    return name;
}

std::string System::getCompilerName()
{
    std::string name = BOOST_COMPILER;
    if(name.empty())
        name = "Unknown Compiler";
    return name;
}

bool System::execute(const bfs::path& command, const std::string& arguments)
{
    // Early exit
    if(!bfs::exists(command))
        return false;
    // Windows has problems with system() call:
    // - Spaces in the command cannot be easily escaped -> Change to folder and execute command
    // - Need backslashes -> Make preferred
    const bfs::path folder = command.parent_path().make_preferred();
    // Linux needs "./cmd" form, not only "cmd"
    const std::string executable = (bfs::path(".") / command.filename()).string();
    if(executable.find(' ') != std::string::npos)
        throw std::runtime_error("Executable must not contain spaces!");
    ScopedCurrentPathChange tmp(folder);
    int result = bnw::system((executable + " " + arguments).c_str());
    return result == 0; // Assume 0 == OK
}

System::ScopedCurrentPathChange::ScopedCurrentPathChange(const boost::filesystem::path& newCurrentPath)
    : oldCurPath(bfs::current_path())
{
    if(!newCurrentPath.empty())
        bfs::current_path(newCurrentPath);
}

System::ScopedCurrentPathChange::~ScopedCurrentPathChange()
{
    bfs::current_path(oldCurPath);
}
