// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Serializer.h"
#include "BinaryFile.h"

Serializer::Serializer(const void* const data, unsigned initial_size)
    : data_(initial_size, boost::container::default_init), length_(0), pos_(0)
{
    PushRawData(data, initial_size);
}

void Serializer::Clear()
{
    data_.clear();
    length_ = 0;
    pos_ = 0;
}

void Serializer::SetLength(const unsigned length)
{
    EnsureSize(length);
    length_ = length;
    if(pos_ > length_)
        pos_ = length_;
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
        uint8_t curVal = PopUnsignedChar();
        result |= (curVal & 0x7F) << (i * 7);
        if(!(curVal & 0x80))
            return result;
    }
    // Can't have more than 5 bytes
    throw std::runtime_error("Invalid var size entry!");
}

bool Serializer::PopBool()
{
    unsigned char value = PopUnsignedChar();
    assert(value == 0 || value == 1);
    return (value != 0);
}

std::string Serializer::PopString()
{
    std::string str;
    str.resize(PopVarSize());
    PopRawData(&str[0], static_cast<unsigned>(str.length()));
    return str;
}

std::string Serializer::PopLongString()
{
    std::string str;
    str.resize(PopUnsignedInt());
    PopRawData(&str[0], static_cast<unsigned>(str.length()));
    return str;
}
