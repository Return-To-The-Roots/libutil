// Copyright (C) 2001 Gennadiy Rozental
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <boost/version.hpp>

#if BOOST_VERSION < 107000
#    define BOOST_TEST_INFO_SCOPE(context_descr)                                             \
        ::boost::test_tools::tt_detail::context_frame BOOST_JOIN(context_frame_, __LINE__) = \
          ::boost::test_tools::tt_detail::context_frame(BOOST_TEST_LAZY_MSG(context_descr))
#endif
