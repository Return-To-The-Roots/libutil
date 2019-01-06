//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_CSTDIO_H_INCLUDED
#define BOOST_NOWIDE_CSTDIO_H_INCLUDED

#include <cstdio>
#include <boost/config.hpp>
#include <boost/nowide/config.hpp>

namespace boost {
namespace nowide {
#if !defined(BOOST_WINDOWS) && !defined(BOOST_NOWIDE_DOXYGEN)
    using std::fopen;
    using std::freopen;
    using std::remove;
    using std::rename;
#else

///
/// \brief Same as freopen but file_name and mode are UTF-8 strings
///
/// If invalid UTF-8 given, NULL is returned and errno is set to EINVAL
///
BOOST_NOWIDE_DECL FILE *freopen(char const *file_name, char const *mode, FILE *stream);
///
/// \brief Same as fopen but file_name and mode are UTF-8 strings
///
/// If invalid UTF-8 given, NULL is returned and errno is set to EINVAL
///
BOOST_NOWIDE_DECL FILE *fopen(char const *file_name, char const *mode);
BOOST_NOWIDE_DECL FILE *fopen(wchar_t const *file_name, wchar_t const *mode);
///
/// \brief Same as rename but old_name and new_name are UTF-8 strings
///
/// If invalid UTF-8 given, -1 is returned and errno is set to EINVAL
///
BOOST_NOWIDE_DECL int rename(char const *old_name, char const *new_name);
///
/// \brief Same as rename but name is UTF-8 string
///
/// If invalid UTF-8 given, -1 is returned and errno is set to EINVAL
///
BOOST_NOWIDE_DECL int remove(char const *name);
#endif
} // nowide
} // namespace boost

#endif
///
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
