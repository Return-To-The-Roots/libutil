#pragma once

/*
Copyright 2019-2023 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)

From Boost 1.90, adapted to work with earlier Boost versions and use other namespace
*/

#include <array>
#include <iterator>
#include <type_traits>

namespace s25util {

constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

template<class T, std::size_t E = dynamic_extent>
class span;

namespace detail {

    template<class U, class T, class = void>
    struct span_convertible
    {
        static constexpr bool value = false;
    };

    template<class U, class T>
    struct span_convertible<U, T, std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>>>
    {
        static constexpr bool value = true;
    };

    template<class U, class T>
    constexpr bool span_convertible_v = span_convertible<U, T>::value;

    template<std::size_t E, std::size_t N>
    struct span_capacity
    {
        static constexpr bool value = E == s25util::dynamic_extent || E == N;
    };

    template<std::size_t E, std::size_t N>
    constexpr bool span_capacity_v = span_capacity<E, N>::value;

    template<class T, std::size_t E, class U, std::size_t N>
    struct span_compatible
    {
        static constexpr bool value = span_capacity_v<E, N> && span_convertible_v<U, T>;
    };
    template<class T, std::size_t E, class U, std::size_t N>
    constexpr bool span_compatible_v = span_compatible<T, E, U, N>::value;

    template<class T>
    using span_uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

    template<class>
    struct span_is_span
    {
        static constexpr bool value = false;
    };

    template<class T, std::size_t E>
    struct span_is_span<s25util::span<T, E>>
    {
        static constexpr bool value = true;
    };

    template<class T>
    constexpr bool span_is_span_v = span_is_span<T>::value;

    template<class T>
    struct span_is_array
    {
        static constexpr bool value = false;
    };

    template<class T, std::size_t N>
    struct span_is_array<std::array<T, N>>
    {
        static constexpr bool value = true;
    };

    template<class T>
    constexpr bool span_is_array_v = span_is_array<T>::value;

    template<class T>
    using span_ptr = decltype(std::data(std::declval<T&>()));

    template<class, class = void>
    struct span_data
    {};

    template<class T>
    struct span_data<T, std::enable_if_t<std::is_pointer_v<span_ptr<T>>>>
    {
        using type = std::remove_pointer_t<span_ptr<T>>;
    };
    template<class T>
    using span_data_t = typename span_data<T>::type;

    template<class, class, class = void>
    struct span_has_data
    {
        static constexpr bool value = false;
    };

    template<class R, class T>
    struct span_has_data<R, T, std::enable_if_t<span_convertible_v<span_data_t<R>, T>>>
    {
        static constexpr bool value = true;
    };

    template<class R, class T>
    constexpr bool span_has_data_v = span_has_data<R, T>::value;

    template<class, class = void>
    struct span_has_size
    {
        static constexpr bool value = false;
    };

    template<class R>
    struct span_has_size<R, std::enable_if_t<std::is_convertible_v<decltype(std::declval<R&>().size()), std::size_t>>>
    {
        static constexpr bool value = true;
    };

    template<class R>
    constexpr bool span_has_size_v = span_has_size<R>::value;

    template<class R, class T>
    struct span_is_range
    {
        static constexpr bool value =
          (std::is_const_v<T> || std::is_lvalue_reference_v<R>)&&!span_is_span_v<span_uncvref<
            R>> && !span_is_array_v<span_uncvref<R>> && !std::is_array_v<span_uncvref<R>> && span_has_data_v<R, T> && span_has_size_v<R>;
    };

    template<class R, class T>
    constexpr bool span_is_range_v = span_is_range<R, T>::value;

    template<std::size_t E, std::size_t N>
    constexpr bool span_implicit_v = E == s25util::dynamic_extent || N != s25util::dynamic_extent;

    template<class T, std::size_t E, class U, std::size_t N>
    struct span_copyable
    {
        static constexpr bool value = (N == s25util::dynamic_extent || span_capacity_v<E, N>)&&span_convertible_v<U, T>;
    };
    template<class T, std::size_t E, class U, std::size_t N>
    constexpr bool span_copyable_v = span_copyable<T, E, U, N>::value;

    template<std::size_t E, std::size_t O>
    constexpr std::size_t span_sub_v = E == s25util::dynamic_extent ? s25util::dynamic_extent : E - O;

    template<class T, std::size_t E>
    struct span_store
    {
        constexpr span_store(T* p_, std::size_t) noexcept : p(p_) {}
        static constexpr std::size_t n = E;
        T* p;
    };

    template<class T>
    struct span_store<T, s25util::dynamic_extent>
    {
        constexpr span_store(T* p_, std::size_t n_) noexcept : p(p_), n(n_) {}
        T* p;
        std::size_t n;
    };

    template<class T, std::size_t E>
    struct span_bytes
    {
        static constexpr std::size_t value = sizeof(T) * E;
    };

    template<class T>
    struct span_bytes<T, s25util::dynamic_extent>
    {
        static constexpr std::size_t value = s25util::dynamic_extent;
    };

    template<class T, std::size_t E>
    constexpr std::size_t span_bytes_v = span_bytes<T, E>::value;

} // namespace detail

template<class T, std::size_t E>
class span
{
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<T*>;
    using const_reverse_iterator = std::reverse_iterator<const T*>;

    static constexpr std::size_t extent = E;

    template<std::size_t N = E, std::enable_if_t<N == dynamic_extent || N == 0, int> = 0>
    constexpr span() noexcept : s_(0, 0)
    {}

    template<class I, std::enable_if_t<E == dynamic_extent && detail::span_convertible_v<I, T>, int> = 0>
    constexpr span(I* f, size_type c) : s_(f, c)
    {}

    template<class I, std::enable_if_t<E != dynamic_extent && detail::span_convertible_v<I, T>, int> = 0>
    explicit constexpr span(I* f, size_type c) : s_(f, c)
    {}

    template<class I, class L, std::enable_if_t<E == dynamic_extent && detail::span_convertible_v<I, T>, int> = 0>
    constexpr span(I* f, L* l) : s_(f, l - f)
    {}

    template<class I, class L, std::enable_if_t<E != dynamic_extent && detail::span_convertible_v<I, T>, int> = 0>
    explicit constexpr span(I* f, L* l) : s_(f, l - f)
    {}

    template<std::size_t N, std::enable_if_t<detail::span_capacity_v<E, N>, int> = 0>
    constexpr span(std::enable_if_t<true, T> (&a)[N]) noexcept : s_(a, N)
    {}

    template<class U, std::size_t N, std::enable_if_t<detail::span_compatible_v<T, E, U, N>, int> = 0>
    constexpr span(std::array<U, N>& a) noexcept : s_(a.data(), N)
    {}

    template<class U, std::size_t N, std::enable_if_t<detail::span_compatible_v<T, E, const U, N>, int> = 0>
    constexpr span(const std::array<U, N>& a) noexcept : s_(a.data(), N)
    {}

    template<class R, std::enable_if_t<E == dynamic_extent && detail::span_is_range_v<R, T>, int> = 0>
    constexpr span(R&& r) noexcept(noexcept(std::data(r)) && noexcept(r.size())) : s_(std::data(r), r.size())
    {}

    template<class R, std::enable_if_t<E != dynamic_extent && detail::span_is_range_v<R, T>, int> = 0>
    explicit constexpr span(R&& r) noexcept(noexcept(std::data(r)) && noexcept(r.size())) : s_(std::data(r), r.size())
    {}

    template<class U, std::size_t N,
             std::enable_if_t<detail::span_implicit_v<E, N> && detail::span_copyable_v<T, E, U, N>, int> = 0>
    constexpr span(const span<U, N>& s) noexcept : s_(s.data(), s.size())
    {}

    template<class U, std::size_t N,
             std::enable_if_t<!detail::span_implicit_v<E, N> && detail::span_copyable_v<T, E, U, N>, int> = 0>
    explicit constexpr span(const span<U, N>& s) noexcept : s_(s.data(), s.size())
    {}

    template<std::size_t C>
    constexpr span<T, C> first() const
    {
        static_assert(C <= E, "Count <= Extent");
        return span<T, C>(s_.p, C);
    }

    template<std::size_t C>
    constexpr span<T, C> last() const
    {
        static_assert(C <= E, "Count <= Extent");
        return span<T, C>(s_.p + (s_.n - C), C);
    }

    template<std::size_t O, std::size_t C = dynamic_extent>
    constexpr std::enable_if_t<C == dynamic_extent, span<T, detail::span_sub_v<E, O>>> subspan() const
    {
        static_assert(O <= E, "Offset <= Extent");
        return span<T, detail::span_sub_v<E, O>>(s_.p + O, s_.n - O);
    }

    template<std::size_t O, std::size_t C = dynamic_extent>
    constexpr std::enable_if_t<C != dynamic_extent, span<T, C>> subspan() const
    {
        static_assert(O <= E && C <= E - O, "Offset <= Extent && Count <= Extent - Offset");
        return span<T, C>(s_.p + O, C);
    }

    constexpr span<T, dynamic_extent> first(size_type c) const
    {
        return BOOST_CORE_DETAIL_ASSERT(c <= size()), span<T, dynamic_extent>(s_.p, c);
    }

    constexpr span<T, dynamic_extent> last(size_type c) const
    {
        return BOOST_CORE_DETAIL_ASSERT(c <= size()), span<T, dynamic_extent>(s_.p + (s_.n - c), c);
    }

    constexpr span<T, dynamic_extent> subspan(size_type o, size_type c = dynamic_extent) const
    {
        return BOOST_CORE_DETAIL_ASSERT(o <= size() && (c == dynamic_extent || c + o <= size())),
               span<T, dynamic_extent>(s_.p + o, c == dynamic_extent ? s_.n - o : c);
    }

    constexpr size_type size() const noexcept { return s_.n; }

    constexpr size_type size_bytes() const noexcept { return s_.n * sizeof(T); }

    constexpr bool empty() const noexcept { return s_.n == 0; }

    constexpr reference operator[](size_type i) const { return BOOST_CORE_DETAIL_ASSERT(i < size()), s_.p[i]; }

    constexpr reference front() const { return BOOST_CORE_DETAIL_ASSERT(!empty()), *s_.p; }

    constexpr reference back() const { return BOOST_CORE_DETAIL_ASSERT(!empty()), s_.p[s_.n - 1]; }

    constexpr pointer data() const noexcept { return s_.p; }

    constexpr iterator begin() const noexcept { return s_.p; }

    constexpr iterator end() const noexcept { return s_.p + s_.n; }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(s_.p + s_.n); }

    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(s_.p); }

    constexpr const_iterator cbegin() const noexcept { return s_.p; }

    constexpr const_iterator cend() const noexcept { return s_.p + s_.n; }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(s_.p + s_.n); }

    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(s_.p); }

private:
    detail::span_store<T, E> s_;
};

#if defined(BOOST_NO_CXX17_INLINE_VARIABLES)
template<class T, std::size_t E>
constexpr std::size_t span<T, E>::extent;
#endif

#ifdef __cpp_deduction_guides
template<class I, class L>
span(I*, L) -> span<I>;

template<class T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template<class T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<class T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<class R>
span(R &&) -> span<detail::span_data_t<R>>;

template<class T, std::size_t E>
span(span<T, E>) -> span<T, E>;
#endif

#ifdef __cpp_lib_byte
template<class T, std::size_t E>
inline span<const std::byte, detail::span_bytes_v<T, E>> as_bytes(span<T, E> s) noexcept
{
    return span<const std::byte, detail::span_bytes_v<T, E>>(reinterpret_cast<const std::byte*>(s.data()),
                                                             s.size_bytes());
}

template<class T, std::size_t E>
inline std::enable_if_t<!std::is_const_v<T>, span<std::byte, detail::span_bytes_v<T, E>>>
as_writable_bytes(span<T, E> s) noexcept
{
    return span<std::byte, detail::span_bytes_v<T, E>>(reinterpret_cast<std::byte*>(s.data()), s.size_bytes());
}
#endif

} // namespace s25util
