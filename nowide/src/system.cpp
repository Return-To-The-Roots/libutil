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
#include <boost/nowide/stackstring.hpp>
#include <boost/nowide/system.hpp>
#include <errno.h>

BOOST_NOWIDE_DECL int boost::nowide::system(char const *cmd)
{
    if(!cmd)
        return _wsystem(0);
    wstackstring wcmd;
    if(!wcmd.convert(cmd))
    {
        errno = EINVAL;
        return -1;
    }
    return _wsystem(wcmd.c_str());
}
