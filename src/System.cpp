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
#include "StringConversion.h"
#include "getExecutablePath.h"
#include <boost/config.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/nowide/system.hpp>

namespace bfs = boost::filesystem;

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
    int result = boost::nowide::system((executable + " " + arguments).c_str());
    return result == 0; // Assume 0 == OK
}

System::ScopedCurrentPathChange::ScopedCurrentPathChange(const boost::filesystem::path& newCurrentPath) : oldCurPath(bfs::current_path())
{
    if(!newCurrentPath.empty())
        bfs::current_path(newCurrentPath);
}

System::ScopedCurrentPathChange::~ScopedCurrentPathChange()
{
    bfs::current_path(oldCurPath);
}
