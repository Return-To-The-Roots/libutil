// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/StringConversion.h"
#include <boost/test/unit_test.hpp>
#include <cstdint>

using namespace s25util;

BOOST_AUTO_TEST_CASE(ConvertFromString)
{
    BOOST_TEST(fromStringClassic<int>("101234") == 101234);
    BOOST_TEST(fromStringClassic<int>("-101234") == -101234);
    BOOST_TEST(fromStringClassic<unsigned>("101234") == 101234u);
    BOOST_TEST(fromStringClassic<float>("2.125") == 2.125f);
    BOOST_TEST(fromStringClassic<uint8_t>("255") == 255u);
    BOOST_TEST(fromStringClassic<uint8_t>("0") == 0);
    BOOST_TEST(fromStringClassic<int8_t>("127") == 127);
    BOOST_TEST(fromStringClassic<int8_t>("-128") == -128);

    BOOST_TEST(fromStringClassic<uint32_t>("0x0", true) == 0u);
    BOOST_TEST(fromStringClassic<uint32_t>("0", true) == 0u);
    BOOST_TEST(fromStringClassic<uint32_t>("0xF", true) == 0xFu);
    BOOST_TEST(fromStringClassic<uint32_t>("0xAABBCCDD", true) == 0xAABBCCDDu);
    BOOST_TEST(fromStringClassic<uint32_t>("0xaabbccdd", true) == 0xAABBCCDDu);
    BOOST_TEST(fromStringClassic<uint32_t>("AABBCCDD", true) == 0xAABBCCDDu);
}

BOOST_AUTO_TEST_CASE(InvalidIntToStrConversions)
{
    // Whitespace
    BOOST_CHECK_THROW(fromStringClassic<int>(" 10"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<int>("10 "), ConversionError);
    // Invalid chars
    BOOST_CHECK_THROW(fromStringClassic<int>("a10"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<int>("10a"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<int>("10.2"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<float>("2,125"), ConversionError);
    // Negative number
    BOOST_CHECK_THROW(fromStringClassic<unsigned>("-10"), ConversionError);
    // Out of range
    BOOST_CHECK_THROW(fromStringClassic<uint8_t>("256"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<int8_t>("129"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<int8_t>("-129"), ConversionError);

    // Invalid hex
    BOOST_CHECK_THROW(fromStringClassic<unsigned>("0xG"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<unsigned>("G"), ConversionError);
    BOOST_CHECK_THROW(fromStringClassic<unsigned>("x"), ConversionError);
}

BOOST_AUTO_TEST_CASE(ConvertToString)
{
    BOOST_TEST(toStringClassic(101234) == "101234");
    BOOST_TEST(toStringClassic(int(-101234)) == "-101234");
    BOOST_TEST(toStringClassic(unsigned(101234u)) == "101234");
    BOOST_TEST(toStringClassic(float(2.125)) == "2.125");
    BOOST_TEST(toStringClassic(uint8_t(255u)) == "255");
    BOOST_TEST(toStringClassic(uint8_t(0)) == "0");
    BOOST_TEST(toStringClassic(int8_t(127)) == "127");
    BOOST_TEST(toStringClassic(int8_t(-128)) == "-128");

    BOOST_TEST(toStringClassic(uint8_t(0), true) == "0x00");
    BOOST_TEST(toStringClassic(uint16_t(0), true) == "0x0000");
    BOOST_TEST(toStringClassic(uint32_t(0), true) == "0x00000000");
    BOOST_TEST(toStringClassic(uint16_t(0xF), true) == "0x000F");
    BOOST_TEST(toStringClassic(uint32_t(0xAABBCCDDu), true) == "0xAABBCCDD");
}
