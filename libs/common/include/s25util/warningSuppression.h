// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/predef/compiler.h>
#include <boost/preprocessor/stringize.hpp>

// Macro that can be used to suppress unused warnings. Required e.g. for const std::arrays defined in headers
// Don't use this if not absolutely necessary!
#ifdef __GNUC__
#    define SUPPRESS_UNUSED __attribute__((unused))
#else
#    define SUPPRESS_UNUSED
#endif

// Use for functions which use "unfixable" undefined behavior, but which is safe
#if defined(__clang__)
#    define RTTR_ATTRIBUTE_NO_UBSAN(what) __attribute__((no_sanitize(BOOST_PP_STRINGIZE(what))))
#elif defined(__GNUC__)
#    define RTTR_ATTRIBUTE_NO_UBSAN(what) __attribute__((no_sanitize_undefined))
#else
#    define RTTR_ATTRIBUTE_NO_UBSAN(what)
#endif

#if BOOST_COMP_CLANG
#    define RTTR_DIAGNOSTIC(what) _Pragma(BOOST_PP_STRINGIZE(clang diagnostic what))
#elif BOOST_COMP_MSVC || defined(__INTELLISENSE__)
#    define RTTR_DIAGNOSTIC(what)
#else
#    define RTTR_DIAGNOSTIC(what) _Pragma(BOOST_PP_STRINGIZE(GCC diagnostic what))
#endif

#define RTTR_PUSH_DIAGNOSTIC RTTR_DIAGNOSTIC(push)
#define RTTR_POP_DIAGNOSTIC RTTR_DIAGNOSTIC(pop)
#define RTTR_IGNORE_DIAGNOSTIC(warning) \
    RTTR_PUSH_DIAGNOSTIC                \
    RTTR_DIAGNOSTIC(ignored warning)
#define RTTR_IGNORE_UNREACHABLE_CODE RTTR_IGNORE_DIAGNOSTIC("-Wunreachable-code")
#define RTTR_IGNORE_OVERLOADED_VIRTUAL RTTR_IGNORE_DIAGNOSTIC("-Woverloaded-virtual")

/// Silence unused variable warning
#define RTTR_UNUSED(x) (void)(x)
