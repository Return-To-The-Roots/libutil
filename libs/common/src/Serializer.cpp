// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Serializer.h"
#include "BinaryFile.h"

Serializer::Serializer() : storage_(boost::container::vector<uint8_t>()) {}

Serializer::Serializer(const void* data, unsigned numBytes)
    : data_(reinterpret_cast<uint8_t*>(const_cast<void*>(data))), length_(numBytes)
{}

void Serializer::clear()
{
    if(storage_)
        storage_->clear();
    data_ = nullptr;
    length_ = 0;
    readPos_ = 0;
}

void Serializer::setLength(const unsigned length)
{
    ensureSize(length);
    length_ = length;
    if(readPos_ > length_)
        readPos_ = length_;
}

void Serializer::ensureSize(const unsigned numBytes)
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

void Serializer::writeToFile(BinaryFile& file) const
{
    file.writeUnsignedInt(getLength());
    file.writeRawData(getData(), getLength());
}

void Serializer::readFromFile(BinaryFile& file)
{
    clear();

    const auto buffer_size = file.readUnsignedInt();
    file.readRawData(getDataWritable(buffer_size), buffer_size);
}

void Serializer::pushVarSize(uint32_t i)
{
    do
    {
        // 7 bit at a time
        uint8_t curVal = i & 0x7F;
        i >>= 7;
        // Anything left? Set highest bit
        if(i)
            curVal |= 0x80;
        push(curVal);
    } while(i);
}

void Serializer::pushBool(bool b)
{
    pushUnsignedChar(b ? 1 : 0);
}

void Serializer::pushString(const std::string& str)
{
    pushVarSize(static_cast<unsigned>(str.length()));
    pushRawData(str.c_str(), static_cast<unsigned>(str.length()));
}

void Serializer::pushLongString(const std::string& str)
{
    pushUnsignedInt(static_cast<unsigned>(str.length()));
    pushRawData(str.c_str(), static_cast<unsigned>(str.length()));
}

uint32_t Serializer::popVarSize()
{
    uint32_t result = 0;
    for(int i = 0; i < 5; i++)
    {
        const auto curVal = popUnsignedChar();
        result |= (curVal & 0x7F) << (i * 7);
        if(!(curVal & 0x80))
            return result;
    }
    // Can't have more than 5 bytes
    throw std::runtime_error("Invalid var size entry!");
}

bool Serializer::popBool()
{
    const auto value = popUnsignedChar();
    assert(value == 0 || value == 1);
    return (value != 0);
}

std::string Serializer::popString()
{
    std::string str;
    str.resize(popVarSize());
    popRawData(str.data(), static_cast<unsigned>(str.length()));
    return str;
}

std::string Serializer::popLongString()
{
    std::string str;
    str.resize(popUnsignedInt());
    popRawData(str.data(), static_cast<unsigned>(str.length()));
    return str;
}

const uint8_t* Serializer::popAndDiscard(unsigned numBytes)
{
    const auto* const data = data_ + readPos_;
    checkSize(numBytes);
    readPos_ += numBytes;
    return data;
}
