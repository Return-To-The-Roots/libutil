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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "getExecutablePath.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef.h>
#include <cstdio>
#include <cstdlib>

// Heavily based on https://stackoverflow.com/a/34109000/1930508 by "Ben Key"

std::string getExecutablePathFallback(const std::string& argv0);

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS) // {

#include <windows.h>
#include <vector>
#include "ucString.h"

std::string getExecutablePath(const std::string& argv0)
{
    std::vector<wchar_t> buf(MAX_PATH);
    while(true)
    {
        DWORD ret = GetModuleFileNameW(NULL, &buf[0], static_cast<DWORD>(buf.size()));
        if(ret == 0)
            return getExecutablePathFallback(argv0);
        else if(ret == static_cast<DWORD>(buf.size()))
            buf.resize(buf.size() * 2);
        else break;
    }
    return cvWideStringToUTF8(&buf[0]);
}

#elif (BOOST_OS_MACOS) // } {

#  include <mach-o/dyld.h>

std::string getExecutablePath(const std::string& argv0)
{
    std::vector<char> buf(1024, 0);
    uint32_t size = buf.size();
    int ret = _NSGetExecutablePath(&buf[0], &size);
    if(ret)
    {
        return getExecutablePathFallback(argv0);
    }
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(&buf[0], ec));
    return p.make_preferred().string();
}

#elif (BOOST_OS_SOLARIS) // } {

#  include <stdlib.h>

std::string getExecutablePath(const std::string& argv0)
{
    std::string ret = getexecname();
    if(ret.empty())
    {
        return getExecutablePathFallback(argv0);
    }
    bfs::path p(ret);
    if(!p.has_root_directory())
    {
        boost::system::error_code ec;
        p = bfs::canonical(p, ec);
        ret = p.make_preferred().string();
    }
    return ret;
}

#elif (BOOST_OS_BSD) // } {

#  include <sys/sysctl.h>

std::string getExecutablePath(const std::string& argv0)
{
    int mib[4] = { 0 };
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    std::vector<char> buf(1024, 0);
    size_t size = buf.size();
    sysctl(mib, 4, &buf[0], &size, NULL, 0);
    if(size == 0 || size == buf.size())
    {
        return getExecutablePathFallback(argv0);
    }
    std::string path(buf.begin(), buf.begin() + size);
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(path, ec));
    return p.make_preferred().string();
}

#elif (BOOST_OS_LINUX) // } {

#  include <unistd.h>

std::string getExecutablePath(const std::string& argv0)
{
    std::vector<char> buf(1024,0);
    size_t size = readlink("/proc/self/exe", &buf[0], buf.size());
    if(size == 0 || size == buf.size())
    {
        return getExecutablePathFallback(argv0);
    }
    std::string path(buf.begin(), buf.begin() + size);
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(path, ec));
    return p.make_preferred().string();
}

#else // } {

std::string getExecutablePath(const std::string& argv0)
{
    return getExecutablePathFallback(argv0);
}

#endif // }

std::string getExecutablePathFallback(const std::string& argv0)
{
    if(argv0.empty())
        return "";
    boost::system::error_code ec;
    bfs::path p(bfs::canonical(argv0, ec));
    return p.make_preferred().string();
}
