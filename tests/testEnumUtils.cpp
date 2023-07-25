// Copyright (C) 2005 - 2023 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/enumUtils.h"
#include <boost/test/unit_test.hpp>
#include <sstream>

enum class InvalidBitset : int
{
};
template<>
struct IsBitset<InvalidBitset> : std::true_type
{};

enum class Bitset : unsigned
{
    None,
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2
};
MAKE_BITSET_STRONG(Bitset);

// Check type traits
static_assert(IsBitset<InvalidBitset>::value);
static_assert(!IsValidBitset<InvalidBitset>);

static_assert(IsBitset<Bitset>::value);
static_assert(IsValidBitset<Bitset>);

BOOST_AUTO_TEST_SUITE(EnumUtils)

BOOST_AUTO_TEST_CASE(Operators)
{
    BOOST_REQUIRE(static_cast<unsigned>(Bitset{}) == 0);

    {
        Bitset b{};
        b = b | Bitset::A;
        BOOST_TEST(static_cast<unsigned>(b) == 0b001u);
        b |= Bitset::B;
        BOOST_TEST(static_cast<unsigned>(b) == 0b011u);
	(b |= Bitset::A) = Bitset::C;
	BOOST_CHECK(b == Bitset::C);
    }

    {
        Bitset b = Bitset::A | Bitset::B | Bitset::C;
        b = b & (Bitset::A | Bitset::B);
        BOOST_TEST(static_cast<unsigned>(b) == 0b011u);
        b &= Bitset::B;
        BOOST_TEST(static_cast<unsigned>(b) == 0b010u);
	(b &= Bitset::A) = Bitset::C;
	BOOST_CHECK(b == Bitset::C);
    }
}

BOOST_AUTO_TEST_CASE(UtilityFunctions)
{
    Bitset b = Bitset::A | Bitset::C;
    BOOST_TEST(bitset::isSet(b, Bitset::A));
    BOOST_TEST(bitset::isSet(b, Bitset::A | Bitset::C));
    BOOST_TEST(!bitset::isSet(b, Bitset::B));
    BOOST_TEST(!bitset::isSet(b, Bitset::B | Bitset::C));

    b = bitset::set(b, Bitset::B /*, true */);
    BOOST_TEST(static_cast<unsigned>(b) == 0b111u);

    b = bitset::set(b, Bitset::B, false);
    BOOST_TEST(static_cast<unsigned>(b) == 0b101u);

    b = bitset::clear(b, Bitset::A);
    BOOST_TEST(static_cast<unsigned>(b) == 0b100u);

    b = bitset::toggle(b, Bitset::A);
    BOOST_TEST(static_cast<unsigned>(b) == 0b101u);

    b = bitset::toggle(b, Bitset::A | Bitset::B);
    BOOST_TEST(static_cast<unsigned>(b) == 0b110u);
}

BOOST_AUTO_TEST_SUITE_END()
