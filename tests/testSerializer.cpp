// Copyright (c) 2016 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "s25util/Serializer.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SerializerSuite)

BOOST_AUTO_TEST_CASE(PushPopSimpleTypes)
{
    uint8_t u8 = 0x10;
    int8_t i8 = 0x11;
    uint16_t u16 = 0x1212;
    int16_t i16 = 0x1313;
    uint32_t u32 = 0x14141414;
    int32_t i32 = 0x15151515;
    uint64_t u64 = 0x1616161616161616;
    int64_t i64 = 0x1717171717171717;
    bool bt = true;
    bool bf = false;
    Serializer ser;
    ser.Push(u8);
    ser.Push(i8);
    ser.Push(u16);
    ser.Push(i16);
    ser.Push(u32);
    ser.Push(i32);
    ser.Push(u64);
    ser.Push(i64);
    ser.Push(bt);
    ser.Push(bf);
    BOOST_TEST_REQUIRE(ser.Pop<uint8_t>() == u8);
    BOOST_TEST_REQUIRE(ser.Pop<int8_t>() == i8);
    BOOST_TEST_REQUIRE(ser.Pop<uint16_t>() == u16);
    BOOST_TEST_REQUIRE(ser.Pop<int16_t>() == i16);
    BOOST_TEST_REQUIRE(ser.Pop<uint32_t>() == u32);
    BOOST_TEST_REQUIRE(ser.Pop<int32_t>() == i32);
    BOOST_TEST_REQUIRE(ser.Pop<uint64_t>() == u64);
    BOOST_TEST_REQUIRE(ser.Pop<int64_t>() == i64);
    BOOST_TEST_REQUIRE(ser.Pop<bool>() == bt);
    BOOST_TEST_REQUIRE(ser.Pop<bool>() == bf);
}

BOOST_AUTO_TEST_CASE(PushPopVarSize)
{
    Serializer ser;
    // Test corner cases of var size
    ser.PushVarSize(0);
    ser.PushVarSize(0x7F);
    ser.PushVarSize(0x80);
    ser.PushVarSize(0x3FFF);
    ser.PushVarSize(0x4000);
    ser.PushVarSize(0x1FFFFF);
    ser.PushVarSize(0xFFFFFFF);
    ser.PushVarSize(0x10000000);
    ser.PushVarSize(0xFFFFFFFF);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0u);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x7Fu);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x80u);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x3FFFu);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x4000u);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x1FFFFFu);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0xFFFFFFFu);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0x10000000u);
    BOOST_TEST_REQUIRE(ser.PopVarSize() == 0xFFFFFFFFu);
}

BOOST_AUTO_TEST_SUITE_END()
