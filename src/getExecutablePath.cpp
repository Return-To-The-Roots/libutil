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

#include "getExecutablePath.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef/os.h>
#include <cstdlib>
#include <vector>

namespace bfs = boost::filesystem;

// Heavily based on https://stackoverflow.com/a/34109000/1930508 by "Ben Key"
// with inspirations from WhereAmI https://github.com/gpakosz/whereami

#if(BOOST_OS_CYGWIN || BOOST_OS_WINDOWS) // {

#include "ucString.h"
#include <windows.h>
#include <array>

std::string getExecutablePath()
{
    std::vector<wchar_t> buf(MAX_PATH);
    while(true)
    {
        DWORD ret = GetModuleFileNameW(nullptr, &buf[0], static_cast<DWORD>(buf.size()));
        if(ret == 0)
            return "";
        else if(ret == static_cast<DWORD>(buf.size()))
            buf.resize(buf.size() * 2);
        else
            break;
    }
    std::array<wchar_t, MAX_PATH> fullPath;
    if(!_wfullpath(&fullPath.front(), &buf.front(), fullPath.size()))
        return "";
    return cvWideStringToUTF8(&fullPath.front());
}

#elif(BOOST_OS_MACOS)

#include <iostream>
#include <mach-o/dyld.h>

std::string getExecutablePath()
{
    std::vector<char> buf(1024, 0);
    uint32_t size = buf.size();
    if(_NSGetExecutablePath(&buf.front(), &size) == -1)
    {
        buf.resize(size);
        if(!_NSGetExecutablePath(&buf.front(), &size))
            return "";
    }
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(&buf.front(), ec));
    return ec ? "" : p.make_preferred().string();
}

#elif(BOOST_OS_SOLARIS)

std::string getExecutablePath()
{
    std::string ret = getexecname();
    if(ret.empty())
        return "";
    bfs::path p(ret);
    if(!p.has_root_directory())
    {
        boost::system::error_code ec;
        p = bfs::canonical(p, ec);
        ret = (ec) ? "" : p.make_preferred().string();
    }
    return ret;
}

#elif(BOOST_OS_BSD)

#include <sys/sysctl.h>

std::string getExecutablePath()
{
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
    std::vector<char> buf(1024, 0);
    size_t size = buf.size();
    if(sysctl(mib, 4, &buf[0], &size, nullptr, 0) != 0)
        return "";
    if(size == 0 || size == buf.size())
        return "";
    std::string path(buf.begin(), buf.begin() + size);
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(path, ec));
    return ec ? "" : p.make_preferred().string();
}

#elif(BOOST_OS_LINUX)

#include <unistd.h>

std::string getExecutablePath()
{
    std::vector<char> buf(1024, 0);
    int size;
    while(true)
    {
        size = readlink("/proc/self/exe", &buf[0], buf.size());
        if(size <= 0)
            return "";
        if(size < static_cast<int>(buf.size()))
            break;
        buf.resize(buf.size() * 2);
    }
    std::string path(buf.begin(), buf.begin() + size);
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(path, ec));
    return ec ? "" : p.make_preferred().string();
}

#else

#error Unsupported plattform!

#endif
