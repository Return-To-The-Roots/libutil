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

#include "libutil/Tokenizer.h"
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
    BOOST_TEST(t.explode() == (std::vector<std::string>{"a1", "b123", "c1234", "d12", "f1"}), boost::test_tools::per_element());
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
