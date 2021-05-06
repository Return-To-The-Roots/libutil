// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/IP.hpp"
#include <boost/test/unit_test.hpp>
#include <stdexcept>

using namespace s25util;

BOOST_AUTO_TEST_CASE(MakeIP)
{
    BOOST_TEST(makeIP(0, 0, 0, 0) == 0u);
    BOOST_TEST(makeIP(1, 2, 3, 4) == 0x01020304u);
    BOOST_TEST(makeIP(0xAB, 0x42, 0xFF, 0x13) == 0xAB42FF13u);
}

BOOST_AUTO_TEST_CASE(ConvertValidIP)
{
    BOOST_TEST(stringToIP("0.0.0.0") == 0u);
    BOOST_TEST(stringToIP("1.2.3.4") == makeIP(1, 2, 3, 4));
    BOOST_TEST(stringToIP("127.168.178.4") == makeIP(127, 168, 178, 4));
    BOOST_TEST(stringToIP("255.255.255.255") == makeIP(255, 255, 255, 255));
    BOOST_TEST(stringToIP("13.75.127.205") == makeIP(13, 75, 127, 205));
}

BOOST_AUTO_TEST_CASE(InvalidIP)
{
    // To short/long
    BOOST_CHECK_THROW(stringToIP("0"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0."), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0."), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.0."), std::invalid_argument);
    // Out of range
    BOOST_CHECK_THROW(stringToIP("256.0.0.0"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.256"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.4096"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("-1.0.0.0"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.-1"), std::invalid_argument);
    // Invalid chars
    BOOST_CHECK_THROW(stringToIP("0.0.0.a"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.+1"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.z"), std::invalid_argument);
    BOOST_CHECK_THROW(stringToIP("0.0.0.{"), std::invalid_argument);
}
