// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Serializer.h"
#include "BinaryFile.h"

Serializer::Serializer() : storage_(boost::container::vector<uint8_t>()) {}

Serializer::Serializer(const void* data, unsigned numBytes)
    : data_(reinterpret_cast<uint8_t*>(const_cast<void*>(data))), length_(numBytes)
{}

void Serializer::Clear()
{
    if(storage_)
        storage_->clear();
    data_ = nullptr;
    length_ = 0;
    readPos_ = 0;
}

void Serializer::SetLength(const unsigned length)
{
    EnsureSize(length);
    length_ = length;
    if(readPos_ > length_)
        readPos_ = length_;
}

void Serializer::EnsureSize(const unsigned numBytes)
{
    if(!storage_)
        throw std::logic_error("Cannot write to deserializer");
    if(storage_->size() < numBytes)
    {
        size_t newSize = 8u;
        while(newSize < numBytes)
            newSize *= 2u;
        storage_->resize(newSize, boost::container::default_init);
        data_ = storage_->data();
    }
}

void Serializer::WriteToFile(BinaryFile& file) const
{
    file.WriteUnsignedInt(GetLength());
    file.WriteRawData(GetData(), GetLength());
}

void Serializer::ReadFromFile(BinaryFile& file)
{
    Clear();

    const auto buffer_size = file.ReadUnsignedInt();
    file.ReadRawData(GetDataWritable(buffer_size), buffer_size);
}

void Serializer::PushVarSize(uint32_t i)
{
    do
    {
        // 7 bit at a time
        uint8_t curVal = i & 0x7F;
        i >>= 7;
        // Anything left? Set highest bit
        if(i)
            curVal |= 0x80;
        Push(curVal);
    } while(i);
}

void Serializer::PushBool(bool b)
{
    PushUnsignedChar(b ? 1 : 0);
}

void Serializer::PushString(const std::string& str)
{
    PushVarSize(static_cast<unsigned>(str.length()));
    PushRawData(str.c_str(), static_cast<unsigned>(str.length()));
}

void Serializer::PushLongString(const std::string& str)
{
    PushUnsignedInt(static_cast<unsigned>(str.length()));
    PushRawData(str.c_str(), static_cast<unsigned>(str.length()));
}

uint32_t Serializer::PopVarSize()
{
    uint32_t result = 0;
    for(int i = 0; i < 5; i++)
    {
        const auto curVal = PopUnsignedChar();
        result |= (curVal & 0x7F) << (i * 7);
        if(!(curVal & 0x80))
            return result;
    }
    // Can't have more than 5 bytes
    throw std::runtime_error("Invalid var size entry!");
}

bool Serializer::PopBool()
{
    const auto value = PopUnsignedChar();
    assert(value == 0 || value == 1);
    return (value != 0);
}

std::string Serializer::PopString()
{
    std::string str;
    str.resize(PopVarSize());
    PopRawData(str.data(), static_cast<unsigned>(str.length()));
    return str;
}

std::string Serializer::PopLongString()
{
    std::string str;
    str.resize(PopUnsignedInt());
    PopRawData(str.data(), static_cast<unsigned>(str.length()));
    return str;
}

const uint8_t* Serializer::PopAndDiscard(unsigned numBytes)
{
    const auto* const data = data_ + readPos_;
    CheckSize(numBytes);
    readPos_ += numBytes;
    return data;
}
