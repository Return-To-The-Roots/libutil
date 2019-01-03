// Copyright (c) 2017 - 2018 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef unique_ptr_h__
#define unique_ptr_h__

#include <boost/version.hpp>
#if BOOST_VERSION >= 105700
#include <boost/move/unique_ptr.hpp>
#else
#include "Deleter.h"
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/remove_extent.hpp>
#include <boost/utility/enable_if.hpp>
#endif

namespace libutil {
#if BOOST_VERSION >= 105700
using boost::movelib::unique_ptr;
#else

namespace detail {
    template<typename T, class D>
    struct unique_base
    {
        typedef boost::interprocess::unique_ptr<typename boost::remove_extent<T>::type, D> type;
    };
} // namespace detail
template<typename T, class D = Deleter<T> >
struct unique_ptr : detail::unique_base<T, D>::type
{
    typedef typename detail::unique_base<T, D>::type base;
    typedef typename base::pointer pointer;
    typedef typename boost::mpl::if_<boost::is_reference<D>, D, typename boost::add_reference<const D>::type>::type DeleterParam;
    unique_ptr() {}
    explicit unique_ptr(pointer p) : base(p) {}
    unique_ptr(pointer p, DeleterParam d) : base(p, d) {}

    unique_ptr(BOOST_RV_REF(unique_ptr) u) : base(static_cast<BOOST_RV_REF(base)>(u)) {}
    unique_ptr& operator=(BOOST_RV_REF(unique_ptr) u)
    {
        base::operator=(static_cast<BOOST_RV_REF(base)>(u));
        return *this;
    }
    template<typename U, typename E>
    unique_ptr(BOOST_RV_REF_BEG unique_ptr<U, E> BOOST_RV_REF_END u)
        : base(static_cast<BOOST_RV_REF_BEG typename detail::unique_base<U, E>::type BOOST_RV_REF_END>(u))
    {}
    template<typename U, typename E>
    unique_ptr& operator=(BOOST_RV_REF_BEG unique_ptr<U, E> BOOST_RV_REF_END u)
    {
        base::operator=(static_cast<BOOST_RV_REF_BEG typename detail::unique_base<U, E>::type BOOST_RV_REF_END>(u));
        return *this;
    }

private:
    BOOST_MOVABLE_BUT_NOT_COPYABLE(unique_ptr)
};
#endif

template<typename T>
unique_ptr<T> make_unique()
{
    return unique_ptr<T>(new T);
}
template<typename T, typename U1>
unique_ptr<T> make_unique(BOOST_FWD_REF(U1) u1)
{
    return unique_ptr<T>(new T(boost::forward<U1>(u1)));
}
template<typename T, typename U1, typename U2>
unique_ptr<T> make_unique(BOOST_FWD_REF(U1) u1, BOOST_FWD_REF(U2) u2)
{
    return unique_ptr<T>(new T(boost::forward<U1>(u1), boost::forward<U2>(u2)));
}
#ifdef BOOST_NO_CXX11_RVALUE_REFERENCES
template<typename T, typename U1>
unique_ptr<T> make_unique(U1& u1)
{
    return unique_ptr<T>(new T(u1));
}
template<typename T, typename U1, typename U2>
unique_ptr<T> make_unique(U1& u1, BOOST_FWD_REF(U2) u2)
{
    return unique_ptr<T>(new T(u1, boost::forward<U2>(u2)));
}
template<typename T, typename U1, typename U2>
unique_ptr<T> make_unique(BOOST_FWD_REF(U1) u1, U2& u2)
{
    return unique_ptr<T>(new T(boost::forward<U1>(u1), u2));
}
template<typename T, typename U1, typename U2>
unique_ptr<T> make_unique(U1& u1, U2& u2)
{
    return unique_ptr<T>(new T(u1, u2));
}

#define RTTR_MOVE_RET(value) boost::move(value)
#else
#define RTTR_MOVE_RET(value) value
#endif

} // namespace libutil

#endif // unique_ptr_h__
