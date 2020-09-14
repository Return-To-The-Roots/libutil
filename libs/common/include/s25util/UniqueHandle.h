// Copyright (c) 2019 - 2019 Settlers Freaks (sf-team at siedler25.org)
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
