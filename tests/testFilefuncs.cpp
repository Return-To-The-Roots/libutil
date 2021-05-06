// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/fileFuncs.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(PortableName)
{
    // Should be: windows_name(name) && portable_posix_name(name) && (name is "." or "..", and the first character not a
    // period or hyphen) windows_name: !name.empty() && (name is "." or ".."  or does not end with a trailing space or
    // period)
    //               The allowed characters are anything except 0x0-0x1F, '<', '>', ':', '"', '/', '\', and '|'.
    // portable_posix_name: !name.empty() The allowed characters are 0-9, a-z, A-Z, '.', '_', and '-'
    BOOST_TEST(makePortableName(".") == ".");
    BOOST_TEST(makePortableName("..") == "..");
    BOOST_TEST(makePortableName("...") == "");
    BOOST_TEST(makePortableName("abc[]!$<>:\"/\\|e") == "abc___________e");
    BOOST_TEST(makePortableName(".abc") == "abc");
    BOOST_TEST(makePortableName("~abc") == "_abc");
    BOOST_TEST(makePortableName("abc ") == "abc_");
    BOOST_TEST(makePortableName("abc.") == "abc");
}

BOOST_AUTO_TEST_CASE(PortableFileName)
{
    BOOST_TEST(makePortableFileName(".") == "");
    BOOST_TEST(makePortableFileName("..") == "");
    BOOST_TEST(makePortableFileName("abc[]!$.exe") == "abc____.exe");
    BOOST_TEST(makePortableFileName("...abc") == "abc");
    BOOST_TEST(makePortableFileName("file.ex1.ex2.ex3") == "fileex1ex2.ex3");
    BOOST_TEST(makePortableFileName("file.extLONG") == "file.ext");
    BOOST_TEST(makePortableFileName("file....") == "file");
}

BOOST_AUTO_TEST_CASE(PortableDirName)
{
    BOOST_TEST(makePortableDirName(".") == ".");
    BOOST_TEST(makePortableDirName("..") == "..");
    BOOST_TEST(makePortableDirName("...") == "");
    BOOST_TEST(makePortableDirName("abc[]!$.exe") == "abc____exe");
    BOOST_TEST(makePortableDirName("...abc") == "abc");
    BOOST_TEST(makePortableDirName("file.ex1.ex2.ex3") == "fileex1ex2ex3");
    BOOST_TEST(makePortableDirName("file.extLONG") == "fileextLONG");
    BOOST_TEST(makePortableDirName("file....") == "file");
}
