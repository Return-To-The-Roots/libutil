// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/md5.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE(Fixed_Cases)
{
    BOOST_TEST(s25util::md5("").toString() == "d41d8cd98f00b204e9800998ecf8427e");
    BOOST_TEST(s25util::md5("Test").toString() == "0cbc6611f5540bd0809a388dc95a615b");
    BOOST_TEST(s25util::md5("SomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongInputSomeVeryLongIn"
                            "putSomeVeryLongInputSomeVeryLongInput")
                 .toString()
               == "4f5586fbeb407ffd0d5e23762167ac67");
    BOOST_TEST(s25util::md5("64CharsOfInput56789012345678901234567890123456789012345678901234").toString()
               == "c123841d524187153dd686e0f87bde7d");
    BOOST_TEST(s25util::md5("64CharsOfInput5678901234567890123456789012345678901234567890123464CharsOfInput5678"
                            "9012345678901234567890123456789012345678901234")
                 .toString()
               == "65a5f6c7d0925bcaad324108046a5644");
}

BOOST_AUTO_TEST_CASE(StreamOp)
{
    std::stringstream ss;
    ss << s25util::md5("Test");
    BOOST_TEST(ss.str() == "0cbc6611f5540bd0809a388dc95a615b");
}

BOOST_AUTO_TEST_CASE(ProcessAdd)
{
    s25util::md5 inst1("");
    inst1.process("Test", true);
    BOOST_TEST(inst1.toString() == s25util::md5("Test").toString());

    inst1 = s25util::md5("Str1");
    s25util::md5 inst2 = inst1;
    inst1.process("Str2", true);
    inst2.process("Str2", false);
    BOOST_TEST(inst1.toString() == s25util::md5("Str1Str2").toString());
    BOOST_TEST(inst2.toString() == s25util::md5("Str2").toString());
}
