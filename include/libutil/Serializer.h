// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#pragma once

#include "libendian/ConvertEndianess.h"
#include <boost/container/vector.hpp>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>

class BinaryFile;

/// Klasse die einen Buffer zum Serialisieren verwaltet und entsprechende Methoden zum Lesen/Schreiben bereitstellt.
/// Implementiert einen FIFO (push fügt ans ende, pop entfernt am anfang)
class Serializer
{
    typedef libendian::ConvertEndianess<true> Converter;

public:
    Serializer();
    Serializer(const Serializer& other);
    Serializer(const void* const data, unsigned initial_size);

    virtual ~Serializer();

    Serializer& operator=(const Serializer& other);

    /// Aufräumen
    void Clear();

    unsigned GetPos() const { return pos_; }

    /// Getter
    unsigned GetLength() const { return length_; }

    /// Schreibzugriff auf die Länge
    void SetLength(const unsigned length);

    unsigned GetBytesLeft() const;

    /// Zugriff auf die Rohdaten
    const unsigned char* GetData() const { return data_.data(); }

    /// Schreibzugriff auf die Rohdaten
    unsigned char* GetDataWritable(unsigned length)
    {
        EnsureSize(length);
        return data_.data();
    }

    /// Schreibt den Buffer in eine Datei
    void WriteToFile(BinaryFile& file);
    /// Liest den Buffer aus einer Datei
    virtual void ReadFromFile(BinaryFile& file);

    void PushRawData(const void* data, unsigned length);

    /// Sämtliche Integer
    void PushSignedInt(int32_t i) { Push(i); }
    void PushUnsignedInt(uint32_t i) { Push(i); }

    void PushSignedShort(int16_t i) { Push(i); }
    void PushUnsignedShort(uint16_t i) { Push(i); }

    void PushSignedChar(int8_t i) { Push(i); }
    void PushUnsignedChar(uint8_t i) { Push(i); }

    void PushBool(bool b);

    void PushString(const std::string& str);

    // Lesemethoden

    /// Rohdaten kopieren
    void PopRawData(void* data, unsigned length);

    /// Sämtliche Integer
    signed int PopSignedInt() { return Pop<int32_t>(); }
    unsigned PopUnsignedInt() { return Pop<uint32_t>(); }

    signed short PopSignedShort() { return Pop<int16_t>(); }
    unsigned short PopUnsignedShort() { return Pop<uint16_t>(); }

    signed char PopSignedChar() { return Pop<int8_t>(); }
    unsigned char PopUnsignedChar() { return Pop<uint8_t>(); }

    bool PopBool();

    std::string PopString();

protected:
    template<typename T>
    T Pop();

    template<typename T>
    void Push(T i);

    /// Erweitert ggf. Speicher um add_length
    void ExtendMemory(unsigned add_length) { EnsureSize(length_ + add_length); }

    /// Makes sure the internal buffer is at least length bytes long
    void EnsureSize(unsigned length);

private:
    /// data mit den Daten
    boost::container::vector<uint8_t> data_;
    /// Logische Länge
    unsigned length_;
    /// Schreib/Leseposition
    unsigned pos_;

    /// Checks if data of size len can be popped
    void CheckSize(unsigned len) const;
};

inline unsigned Serializer::GetBytesLeft() const
{
    assert(pos_ <= length_);
    return length_ - pos_;
}

inline void Serializer::EnsureSize(unsigned length)
{
    if(data_.size() < length)
    {
        data_.reserve(length);
        data_.resize(data_.capacity(), boost::container::default_init);
    }
}

inline void Serializer::CheckSize(unsigned len) const
{
    if(GetBytesLeft() < len)
        throw std::range_error("Out of range during deserialization");
}

inline void Serializer::PushRawData(const void* data, unsigned length)
{
    if(length == 0)
        return;
    ExtendMemory(length);
    std::memcpy(&data_[length_], data, length);
    this->length_ += length;
}

inline void Serializer::PopRawData(void* data, unsigned length)
{
    if(length == 0)
        return;
    CheckSize(length);
    std::memcpy(data, &data_[pos_], length);
    pos_ += length;
}

template<typename T>
inline T Serializer::Pop()
{
    if(sizeof(T) == 1)
    {
        CheckSize(sizeof(T));
        T i = static_cast<T>(data_[pos_]);
        pos_ += sizeof(T);
        return i;
    }
    T i;
    // Note: No casting allowed due to alignment
    PopRawData(&i, sizeof(i));
    return Converter::toNative(i);
}

template<typename T>
inline void Serializer::Push(T i)
{
    if(sizeof(T) == 1)
    {
        ExtendMemory(sizeof(T));
        data_[length_] = static_cast<uint8_t>(i);
        length_ += sizeof(T);
    } else
    {
        i = Converter::fromNative(i);
        PushRawData(&i, sizeof(i));
    }
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

#endif // !SERIALIZER_H_INCLUDED
