// Copyright (C) 2001 Gennadiy Rozental
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <boost/version.hpp>
#include <iosfwd>
#include <memory>

#if BOOST_VERSION < 107000
#    define BOOST_TEST_INFO_SCOPE(context_descr)                                             \
        ::boost::test_tools::tt_detail::context_frame BOOST_JOIN(context_frame_, __LINE__) = \
          ::boost::test_tools::tt_detail::context_frame(BOOST_TEST_LAZY_MSG(context_descr))
#endif

namespace std {
template<class T, class D>
std::ostream& boost_test_print_type(std::ostream& os, const std::unique_ptr<T, D>& p)
{
    return os << p.get();
}
template<class T>
std::ostream& boost_test_print_type(std::ostream& os, const std::shared_ptr<T>& p)
{
    return os << p.get();
}
} // namespace std
