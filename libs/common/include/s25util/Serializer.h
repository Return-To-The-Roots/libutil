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
    void clear();
    /// Get current read position
    unsigned getReadPos() const noexcept { return readPos_; }

    /// Get current number of bytes contained
    unsigned getLength() const noexcept { return length_; }
    /// Set the current length
    void setLength(unsigned length);
    /// Get remaining bytes to read
    unsigned getBytesLeft() const noexcept;

    /// Access current data
    const uint8_t* getData() const noexcept { return data_; }

    /// Change the size of the buffer and provide writable access it
    uint8_t* getDataWritable(unsigned length)
    {
        setLength(length);
        return data_;
    }

    void writeToFile(BinaryFile& file) const;
    void readFromFile(BinaryFile& file);

    // Write methods

    void pushRawData(const void* data, unsigned numBytes);

    void pushSignedInt(int32_t i) { push(i); }
    void pushUnsignedInt(uint32_t i) { push(i); }

    void pushSignedShort(int16_t i) { push(i); }
    void pushUnsignedShort(uint16_t i) { push(i); }

    void pushSignedChar(int8_t i) { push(i); }
    void pushUnsignedChar(uint8_t i) { push(i); }
    /// push a variable sized uint32_t. Uses less space when value is small
    void pushVarSize(uint32_t i);

    void pushBool(bool b);

    void pushString(const std::string& str);
    void pushLongString(const std::string& str);

    // Read methods

    void popRawData(void* data, unsigned numBytes);

    int32_t popSignedInt() { return pop<int32_t>(); }
    uint32_t popUnsignedInt() { return pop<uint32_t>(); }

    int16_t popSignedShort() { return pop<int16_t>(); }
    uint16_t popUnsignedShort() { return pop<uint16_t>(); }

    int8_t popSignedChar() { return pop<int8_t>(); }
    uint8_t popUnsignedChar() { return pop<uint8_t>(); }
    uint32_t popVarSize();

    bool popBool();

    std::string popString();
    std::string popLongString();

    /// Advance the read pointer by the given number of bytes
    /// Return a pointer to the start of that data in the internal buffer
    const uint8_t* popAndDiscard(unsigned numBytes);

    template<typename T>
    T pop();

    template<typename T>
    void push(T val);

private:
    /// Add the given number of bytes to the usable length
    void extendMemory(unsigned numBytes) { ensureSize(length_ + numBytes); }
    /// Verify that the given number of bytes can be read/popped
    /// Otherwise throws a length_error
    void checkSize(unsigned numBytes) const;
    /// Make sure the internal buffer can hold the given number of bytes
    /// potentially increasing the size
    void ensureSize(unsigned numBytes);

    std::optional<boost::container::vector<uint8_t>> storage_;
    uint8_t* data_ = nullptr;
    /// Number of bytes of valid data.
    /// I.e. the current write position and the total number of readable bytes
    unsigned length_ = 0;
    /// Current read position
    unsigned readPos_ = 0;
};

inline unsigned Serializer::getBytesLeft() const noexcept
{
    assert(readPos_ <= length_);
    return length_ - readPos_;
}

inline void Serializer::checkSize(unsigned numBytes) const
{
    if(getBytesLeft() < numBytes)
        throw std::range_error("Out of range during deserialization");
}

inline void Serializer::pushRawData(const void* data, unsigned numBytes)
{
    if(numBytes == 0)
        return;
    extendMemory(numBytes);
    std::memcpy(&data_[length_], data, numBytes);
    this->length_ += numBytes;
}

inline void Serializer::popRawData(void* data, unsigned numBytes)
{
    if(numBytes == 0)
        return;
    checkSize(numBytes);
    std::memcpy(data, &data_[readPos_], numBytes);
    readPos_ += numBytes;
}

template<typename T>
inline T Serializer::pop()
{
    static_assert(std::is_trivial_v<T> && !std::is_pointer_v<T>, "Type must be a trivial, non-pointer type");
    T i;
    // Note: No casting allowed due to alignment
    popRawData(&i, sizeof(i));
    return Converter::toNative(i);
}

template<typename T>
inline void Serializer::push(T val)
{
    static_assert(std::is_trivial_v<T> && !std::is_pointer_v<T>, "Type must be a trivial, non-pointer type");
    val = Converter::fromNative(val);
    pushRawData(&val, sizeof(val));
}

template<>
inline void Serializer::push(bool val)
{
    pushBool(val);
}

template<>
inline bool Serializer::pop<bool>()
{
    return popBool();
}
