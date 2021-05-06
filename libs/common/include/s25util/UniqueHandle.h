// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <type_traits>
#include <utility>

namespace s25util {

namespace traits {
    /// Return value denotating an invalid handle
    template<class T_Handle>
    struct InvalidHandleValue;

    /// Callable for freeing a handle
    template<class T_Handle>
    struct FreeHandle
    {
        void operator()(T_Handle h) { freeHandle(h); }
    };
} // namespace traits

/// Generalized unique_ptr for handles (any type which references data)
/// Need to define traits::freeHandle(Handle) or specialize traits::FreeHandleand
/// and for non-pointers specialize traits::InvalidHandleValue
template<class T>
class UniqueHandle
{
    using InvalidValue = traits::InvalidHandleValue<T>;
    T data_;

public:
    UniqueHandle() : data_(InvalidValue{}) {}
    explicit UniqueHandle(T data) : data_(data) {}
    UniqueHandle(UniqueHandle&& other) noexcept : data_(std::exchange(other.data_, InvalidValue{})) {}
    UniqueHandle& operator=(UniqueHandle&& other) noexcept
    {
        data_ = std::exchange(other.data_, InvalidValue{});
        return *this;
    }
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    ~UniqueHandle()
    {
        if(data_ != InvalidValue{})
            traits::FreeHandle<T>{}(data_);
    }
    operator T() const { return data_; }
    template<typename U = T, typename = std::enable_if_t<std::is_pointer<U>::value>>
    T operator&()
    {
        return data_;
    }
    template<typename U = T, typename = std::enable_if_t<!std::is_pointer<U>::value>>
    T* operator&()
    {
        return &data_;
    }
    template<typename U = T, typename = std::enable_if_t<std::is_pointer<U>::value>>
    T operator->()
    {
        return data_;
    }
    template<typename U = T, typename = std::enable_if_t<!std::is_pointer<U>::value>>
    T* operator->()
    {
        return &data_;
    }
};

namespace traits {
    template<class T_Ptr>
    struct InvalidHandleValue<T_Ptr*> : std::integral_constant<T_Ptr*, nullptr>
    {};
} // namespace traits

} // namespace s25util
