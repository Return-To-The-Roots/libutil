// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/Tokenizer.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(Extract)
{
    Tokenizer t("a1;b123;c1234 d12.f1", "; .");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.next() == "a1");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.next() == "b123");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.next() == "c1234");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.next() == "d12");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.next() == "f1");
    BOOST_REQUIRE(!t);
    BOOST_TEST(t.next() == "");
    BOOST_TEST(t.next() == "");

    // Split(foo) == [foo]
    for(const auto& singleStr : {"", "foobar", "b.c d"})
    {
        t = Tokenizer(singleStr, ";");
        BOOST_REQUIRE(t);
        BOOST_TEST(t.next() == singleStr);
        BOOST_REQUIRE(!t);
    }
}

BOOST_AUTO_TEST_CASE(Explode)
{
    Tokenizer t("a1;b123;c1234 d12.f1", "; .");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{"a1", "b123", "c1234", "d12", "f1"}),
               boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(ExtractWithEmpty)
{
    Tokenizer t("a;;c;;;f");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{"a", "", "c", "", "", "f"}), boost::test_tools::per_element());

    t = Tokenizer(";a;b");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{"", "a", "b"}), boost::test_tools::per_element());

    t = Tokenizer("a;b;");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{"a", "b", ""}), boost::test_tools::per_element());

    t = Tokenizer(";");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{"", ""}), boost::test_tools::per_element());

    t = Tokenizer("");
    BOOST_REQUIRE(t);
    BOOST_TEST(t.explode() == (std::vector<std::string>{""}), boost::test_tools::per_element());
}
