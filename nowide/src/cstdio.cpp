//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
#define BOOST_NOWIDE_SOURCE
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/nowide/stackstring.hpp>
#include <errno.h>

namespace boost
{
namespace nowide
{
FILE* freopen(char const *file_name, char const *mode, FILE*stream)
{
    wstackstring wname;
    wshort_stackstring wmode;
    if(!wname.convert(file_name) || !wmode.convert(mode))
    {
        errno = EINVAL;
        return 0;
    }
    return _wfreopen(wname.c_str(), wmode.c_str(), stream);
}

FILE* fopen(char const *file_name, char const *mode)
{
    wstackstring wname;
    wshort_stackstring wmode;
    if(!wname.convert(file_name) || !wmode.convert(mode))
    {
        errno = EINVAL;
        return 0;
    }
    return fopen(wname.c_str(), wmode.c_str());
}

FILE* fopen(wchar_t const *file_name, wchar_t const *mode)
{
    return _wfopen(file_name, mode);
}

int rename(char const *old_name, char const *new_name)
{
    wstackstring wold, wnew;
    if(!wold.convert(old_name) || !wnew.convert(new_name))
    {
        errno = EINVAL;
        return -1;
    }
    return _wrename(wold.c_str(), wnew.c_str());
}

int remove(char const *name)
{
    wstackstring wname;
    if(!wname.convert(name))
    {
        errno = EINVAL;
        return -1;
    }
    return _wremove(wname.c_str());
}

}
}
