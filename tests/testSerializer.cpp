// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/Serializer.h"
#include "s25util/VersionedDeserializer.h"
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

struct Deserializer : s25util::VersionedDeserializer<Deserializer>
{
    using s25util::VersionedDeserializer<Deserializer>::VersionedDeserializer;
    static unsigned getCurrentVersion() { return 2; }
};

struct Example
{
    struct Sub
    {
        std::pair<unsigned, uint16_t> fieldsV0{01, 02};
        std::pair<unsigned, uint16_t> fieldsV1{11, 12};
        void save(Serializer& ser, const unsigned version) const
        {
            ser.Push(fieldsV0.first);
            ser.Push(fieldsV0.second);
            // Only for testing an old version
            if(version >= 1)
            {
                ser.Push(fieldsV1.first);
                ser.Push(fieldsV1.second);
            }
        }
        void load(Deserializer& ser)
        {
            fieldsV0.first = ser.Pop<unsigned>();
            fieldsV0.second = ser.Pop<uint16_t>();
            if(ser.getDataVersion() >= 1)
            {
                fieldsV1.first = ser.Pop<unsigned>();
                fieldsV1.second = ser.Pop<uint16_t>();
            } else
                fieldsV1 = {11, 12};
        }
    };
    uint32_t field1 = 11111111;
    Sub substruct;
    uint32_t field2 = 22222222;

    auto getFields() const { return std::tie(field1, field2, substruct.fieldsV0, substruct.fieldsV1); }
    bool operator==(const Example& rhs) const { return getFields() == rhs.getFields(); }
    void save(Serializer& ser, const unsigned version) const
    { // Version passed only for testing
        ser.Push(field1);
        ser.Push(version);
        substruct.save(ser, version);
        ser.Push(field2);
    }
    void load(Serializer& ser, const unsigned version)
    {
        field1 = ser.Pop<uint32_t>();
        BOOST_TEST_REQUIRE(ser.Pop<unsigned>() == version);
        {
            Deserializer deser(ser, version);
            substruct.load(deser);
        }
        field2 = ser.Pop<uint32_t>();
    }
};

BOOST_AUTO_TEST_CASE(VersionedDeserializer)
{
    const Example input;
    Serializer ser0, ser1;
    input.save(ser0, 0);
    input.save(ser1, 1);
    const std::vector<uint8_t> v0Data(ser0.GetData(), ser0.GetData() + ser0.GetLength());
    const std::vector<uint8_t> v1Data(ser1.GetData(), ser1.GetData() + ser1.GetLength());
    {
        Example out{};
        Serializer ser(v0Data.data(), v0Data.size());
        out.load(ser, 0);
        BOOST_TEST((out.getFields() == input.getFields()));
    }
    {
        Example out{};
        Serializer ser(v1Data.data(), v1Data.size());
        out.load(ser, 1);
        BOOST_TEST((out.getFields() == input.getFields()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
