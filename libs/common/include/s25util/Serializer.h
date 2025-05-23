// Copyright (C) 2005 - 2025 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "libendian/ConvertEndianess.h"
#include <boost/container/vector.hpp>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

class BinaryFile;

/// Class for serialization implementing a FIFO buffer (push adds to end, pop reads from front)
class Serializer
{
    using Converter = libendian::ConvertEndianess<true>;

public:
    /// Create for reading and writing
    Serializer();
    /// Create for deserializing the given data (read-only, view semantic, does not copy)
    Serializer(const void* data, unsigned numBytes);

    /// Remove all data
    void Clear();
    /// Get current read position
    unsigned GetReadPos() const noexcept { return readPos_; }

    /// Get current number of bytes contained
    unsigned GetLength() const noexcept { return length_; }
    /// Set the current length
    void SetLength(unsigned length);
    /// Get remaining bytes to read
    unsigned GetBytesLeft() const noexcept;

    /// Access current data
    const uint8_t* GetData() const noexcept { return data_; }

    /// Change the size of the buffer and provide writable access it
    uint8_t* GetDataWritable(unsigned length)
    {
        SetLength(length);
        return data_;
    }

    void WriteToFile(BinaryFile& file) const;
    void ReadFromFile(BinaryFile& file);

    // Write methods

    void PushRawData(const void* data, unsigned numBytes);

    void PushSignedInt(int32_t i) { Push(i); }
    void PushUnsignedInt(uint32_t i) { Push(i); }

    void PushSignedShort(int16_t i) { Push(i); }
    void PushUnsignedShort(uint16_t i) { Push(i); }

    void PushSignedChar(int8_t i) { Push(i); }
    void PushUnsignedChar(uint8_t i) { Push(i); }
    /// Push a variable sized uint32_t. Uses less space when value is small
    void PushVarSize(uint32_t i);

    void PushBool(bool b);

    void PushString(const std::string& str);
    void PushLongString(const std::string& str);

    // Read methods

    void PopRawData(void* data, unsigned numBytes);

    int32_t PopSignedInt() { return Pop<int32_t>(); }
    uint32_t PopUnsignedInt() { return Pop<uint32_t>(); }

    int16_t PopSignedShort() { return Pop<int16_t>(); }
    uint16_t PopUnsignedShort() { return Pop<uint16_t>(); }

    int8_t PopSignedChar() { return Pop<int8_t>(); }
    uint8_t PopUnsignedChar() { return Pop<uint8_t>(); }
    uint32_t PopVarSize();

    bool PopBool();

    std::string PopString();
    std::string PopLongString();

    /// Advance the read pointer by the given number of bytes
    /// Return a pointer to the start of that data in the internal buffer
    const uint8_t* PopAndDiscard(unsigned numBytes);

    template<typename T>
    T Pop();

    template<typename T>
    void Push(T val);

private:
    /// Add the given number of bytes to the usable length
    void ExtendMemory(unsigned numBytes) { EnsureSize(length_ + numBytes); }
    /// Verify that the given number of bytes can be read/popped
    /// Otherwise throws a length_error
    void CheckSize(unsigned numBytes) const;
    /// Make sure the internal buffer can hold the given number of bytes
    /// potentially increasing the size
    void EnsureSize(unsigned numBytes);

    std::optional<boost::container::vector<uint8_t>> storage_;
    uint8_t* data_ = nullptr;
    /// Number of bytes of valid data.
    /// I.e. the current write position and the total number of readable bytes
    unsigned length_ = 0;
    /// Current read position
    unsigned readPos_ = 0;
};

inline unsigned Serializer::GetBytesLeft() const noexcept
{
    assert(readPos_ <= length_);
    return length_ - readPos_;
}

inline void Serializer::CheckSize(unsigned numBytes) const
{
    if(GetBytesLeft() < numBytes)
        throw std::range_error("Out of range during deserialization");
}

inline void Serializer::PushRawData(const void* data, unsigned numBytes)
{
    if(numBytes == 0)
        return;
    ExtendMemory(numBytes);
    std::memcpy(&data_[length_], data, numBytes);
    this->length_ += numBytes;
}

inline void Serializer::PopRawData(void* data, unsigned numBytes)
{
    if(numBytes == 0)
        return;
    CheckSize(numBytes);
    std::memcpy(data, &data_[readPos_], numBytes);
    readPos_ += numBytes;
}

template<typename T>
inline T Serializer::Pop()
{
    static_assert(std::is_trivial_v<T> && !std::is_pointer_v<T>, "Type must be a trivial, non-pointer type");
    T i;
    // Note: No casting allowed due to alignment
    PopRawData(&i, sizeof(i));
    return Converter::toNative(i);
}

template<typename T>
inline void Serializer::Push(T val)
{
    static_assert(std::is_trivial_v<T> && !std::is_pointer_v<T>, "Type must be a trivial, non-pointer type");
    val = Converter::fromNative(val);
    PushRawData(&val, sizeof(val));
}

template<>
inline void Serializer::Push(bool val)
{
    PushBool(val);
}

template<>
inline bool Serializer::Pop<bool>()
{
    return PopBool();
}
