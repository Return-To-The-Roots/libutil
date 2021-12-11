// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define BOOST_TEST_MODULE s25util_Test
#include "s25util/boostTestHelpers.h"
#include <boost/test/unit_test.hpp>
#include <memory>
#include <sstream>

//#include <vld.h>

using TestedTypes = std::tuple<std::unique_ptr<int>, std::shared_ptr<int>>;
BOOST_AUTO_TEST_CASE_TEMPLATE(PrintSmartPtr, T, TestedTypes)
{
    std::stringstream sOrig, sBoost;
    T p;
    sOrig << p.get();
    boost_test_print_type(sBoost, p);

    p.reset(new int(42));
    sOrig << p.get();
    boost_test_print_type(sBoost, p);

    BOOST_TEST(sBoost.str() == sOrig.str());
}
