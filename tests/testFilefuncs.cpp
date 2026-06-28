// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
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

    // Reserved names get _ appended
    BOOST_TEST(makePortableName("con") == "con_");
    BOOST_TEST(makePortableName("NUL") == "NUL_");
    BOOST_TEST(makePortableName("com1") == "com1_");
    BOOST_TEST(makePortableName("lpt9") == "lpt9_");
    BOOST_TEST(makePortableName("com0") == "com0_");
    BOOST_TEST(makePortableName("lpt0") == "lpt0_");
    BOOST_TEST(makePortableName("prn") == "prn_");
    BOOST_TEST(makePortableName("aux") == "aux_");
    // Non-reserved names are unchanged
    BOOST_TEST(makePortableName("null") == "null");
    BOOST_TEST(makePortableName("console") == "console");
    BOOST_TEST(makePortableName("com10") == "com10");
    BOOST_TEST(makePortableName("lpt10") == "lpt10");
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

BOOST_AUTO_TEST_CASE(ValidFileNameChar)
{
    // Allowed
    BOOST_TEST(isValidFileNameChar('a'));
    BOOST_TEST(isValidFileNameChar(' '));
    BOOST_TEST(isValidFileNameChar(U'\u00E9')); // non-ASCII Unicode

    // Rejected — Windows-forbidden
    BOOST_TEST(!isValidFileNameChar(':'));
    // Rejected — control character
    BOOST_TEST(!isValidFileNameChar('\0'));
}

BOOST_AUTO_TEST_CASE(ValidFileName)
{
    BOOST_TEST(isValidFileName("my save"));

    // Empty
    BOOST_TEST(!isValidFileName(""));

    // Reserved name (case-insensitive)
    BOOST_TEST(!isValidFileName("con"));
    BOOST_TEST(!isValidFileName("CON"));

    // Non-reserved look-alike
    BOOST_TEST(isValidFileName("null"));

    // Leading/trailing dots
    BOOST_TEST(!isValidFileName(".hidden"));
    BOOST_TEST(!isValidFileName("trail."));
    BOOST_TEST(!isValidFileName("."));
    BOOST_TEST(!isValidFileName(".."));

    // Trailing space
    BOOST_TEST(!isValidFileName("trail "));

    // Reserved base name with extension (Windows 7 compat)
    BOOST_TEST(!isValidFileName("nul.ini"));

    // Invalid character
    BOOST_TEST(!isValidFileName("save:game"));
}