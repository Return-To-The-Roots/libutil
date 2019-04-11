// Copyright (c) 2017 - 2019 Settlers Freaks (sf-team at siedler25.org)
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

#include "libutil/fileFuncs.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(PortableName)
{
    // Should be: windows_name(name) && portable_posix_name(name) && (name is "." or "..", and the first character not a period or hyphen)
    // windows_name: !name.empty() && (name is "." or ".."  or does not end with a trailing space or period)
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
