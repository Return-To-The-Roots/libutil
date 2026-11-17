// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/Serializer.h"
#include "s25util/VersionedDeserializer.h"
#include <boost/test/unit_test.hpp>
#include <stdexcept>

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
    ser.push(u8);
    ser.push(i8);
    ser.push(u16);
    ser.push(i16);
    ser.push(u32);
    ser.push(i32);
    ser.push(u64);
    ser.push(i64);
    ser.push(bt);
    ser.push(bf);
    BOOST_TEST_REQUIRE(ser.pop<uint8_t>() == u8);
    BOOST_TEST_REQUIRE(ser.pop<int8_t>() == i8);
    BOOST_TEST_REQUIRE(ser.pop<uint16_t>() == u16);
    BOOST_TEST_REQUIRE(ser.pop<int16_t>() == i16);
    BOOST_TEST_REQUIRE(ser.pop<uint32_t>() == u32);
    BOOST_TEST_REQUIRE(ser.pop<int32_t>() == i32);
    BOOST_TEST_REQUIRE(ser.pop<uint64_t>() == u64);
    BOOST_TEST_REQUIRE(ser.pop<int64_t>() == i64);
    BOOST_TEST_REQUIRE(ser.pop<bool>() == bt);
    BOOST_TEST_REQUIRE(ser.pop<bool>() == bf);
}

BOOST_AUTO_TEST_CASE(PushPopVarSize)
{
    Serializer ser;
    // Test corner cases of var size
    ser.pushVarSize(0);
    ser.pushVarSize(0x7F);
    ser.pushVarSize(0x80);
    ser.pushVarSize(0x3FFF);
    ser.pushVarSize(0x4000);
    ser.pushVarSize(0x1FFFFF);
    ser.pushVarSize(0xFFFFFFF);
    ser.pushVarSize(0x10000000);
    ser.pushVarSize(0xFFFFFFFF);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0u);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x7Fu);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x80u);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x3FFFu);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x4000u);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x1FFFFFu);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0xFFFFFFFu);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0x10000000u);
    BOOST_TEST_REQUIRE(ser.popVarSize() == 0xFFFFFFFFu);
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
            ser.push(fieldsV0.first);
            ser.push(fieldsV0.second);
            // Only for testing an old version
            if(version >= 1)
            {
                ser.push(fieldsV1.first);
                ser.push(fieldsV1.second);
            }
        }
        void load(Deserializer& ser)
        {
            fieldsV0.first = ser.pop<unsigned>();
            fieldsV0.second = ser.pop<uint16_t>();
            if(ser.getDataVersion() >= 1)
            {
                fieldsV1.first = ser.pop<unsigned>();
                fieldsV1.second = ser.pop<uint16_t>();
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
        ser.push(field1);
        ser.push(version);
        substruct.save(ser, version);
        ser.push(field2);
    }
    void load(Serializer& ser, const unsigned version)
    {
        field1 = ser.pop<uint32_t>();
        BOOST_TEST_REQUIRE(ser.pop<unsigned>() == version);
        {
            Deserializer deser(ser, version);
            substruct.load(deser);
        }
        field2 = ser.pop<uint32_t>();
    }
};

BOOST_AUTO_TEST_CASE(VersionedDeserializer)
{
    const Example input;
    Serializer ser0, ser1;
    input.save(ser0, 0);
    input.save(ser1, 1);
    const std::vector<uint8_t> v0Data(ser0.getData(), ser0.getData() + ser0.getLength());
    const std::vector<uint8_t> v1Data(ser1.getData(), ser1.getData() + ser1.getLength());
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

BOOST_AUTO_TEST_CASE(SerializerView)
{
    Serializer ser;
    const uint32_t value = 1337;
    ser.push(value);
    Serializer deser(ser.getData(), ser.getLength());
    // Can't write
    BOOST_CHECK_THROW(deser.push(42), std::logic_error);
    // Only read
    BOOST_TEST(deser.pop<uint32_t>() == value);
    // No overflow
    BOOST_TEST(deser.getBytesLeft() == 0u);
    BOOST_CHECK_THROW(deser.popBool(), std::range_error);
    // clear allows writing only if we could before.
    // E.g. when the original data goes out of scope but the deserializer does not
    deser.clear();
    BOOST_CHECK_THROW(deser.push(value), std::logic_error);
    BOOST_TEST(ser.getLength() == sizeof(uint32_t));
    ser.clear();
    BOOST_TEST(ser.getLength() == 0u);
    BOOST_REQUIRE_NO_THROW(ser.push<uint16_t>(value));
    BOOST_TEST(ser.pop<uint16_t>() == value);
    BOOST_TEST(ser.getLength() == sizeof(uint16_t));
    // But we can reinit a deserializer
    deser = Serializer();
    BOOST_REQUIRE_NO_THROW(deser.push(value));
    BOOST_TEST(deser.pop<uint32_t>() == value);
    BOOST_TEST(deser.getLength() == sizeof(uint32_t));
}

BOOST_AUTO_TEST_SUITE_END()
