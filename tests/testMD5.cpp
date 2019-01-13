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

#include "libutil/md5.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE(Fixed_Cases)
{
    BOOST_TEST_REQUIRE(s25util::md5("").toString() == "d41d8cd98f00b204e9800998ecf8427e");
    BOOST_TEST_REQUIRE(s25util::md5("Test").toString() == "0cbc6611f5540bd0809a388dc95a615b");
    BOOST_TEST_REQUIRE(s25util::md5("SomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongInput").toString() == "4f5586fbeb407ffd0d5e23762167ac67");
    BOOST_TEST_REQUIRE(s25util::md5("64CharsOfInput56789012345678901234567890123456789012345678901234").toString() == "c123841d524187153dd686e0f87bde7d");
    BOOST_TEST_REQUIRE(s25util::md5("64CharsOfInput5678901234567890123456789012345678901234567890123464CharsOfInput56789012345678901234567890123456789012345678901234").toString() == "65a5f6c7d0925bcaad324108046a5644");    
}

BOOST_AUTO_TEST_CASE(StreamOp)
{
    std::stringstream ss;
    ss << s25util::md5("Test");
    BOOST_TEST_REQUIRE(ss.str() == "0cbc6611f5540bd0809a388dc95a615b");
}

BOOST_AUTO_TEST_CASE(ProcessAdd)
{
    s25util::md5 inst1("");
    inst1.process("Test", true);
    BOOST_TEST_REQUIRE(inst1.toString() == s25util::md5("Test").toString());

    inst1 = s25util::md5("Str1");
    s25util::md5 inst2 = inst1;
    inst1.process("Str2", true);
    inst2.process("Str2", false);
    BOOST_TEST_REQUIRE(inst1.toString() == s25util::md5("Str1Str2").toString());
    BOOST_TEST_REQUIRE(inst2.toString() == s25util::md5("Str2").toString());
}
