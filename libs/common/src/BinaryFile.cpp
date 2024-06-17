// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "BinaryFile.h"
#include <boost/nowide/cstdio.hpp>
#include <array>
#include <limits>
#include <stdexcept>

#define CHECKED_READ(...) \
    if(!(__VA_ARGS__))    \
    throw std::runtime_error("Unknown error during reading " + filePath_.string())
#define CHECKED_WRITE(...) \
    if(!(__VA_ARGS__))     \
    throw std::runtime_error("Unknown error during writing " + filePath_.string())

bool BinaryFile::Open(const boost::filesystem::path& filePath, const OpenFileMode of)
{
    Close();
    static constexpr std::array<const char*, 3> modes = {{"w+b", "a+b", "rb"}};
    file.reset(boost::nowide::fopen(filePath.string().c_str(), modes[static_cast<unsigned>(of)]));
    if(file)
    {
        filePath_ = filePath;
        return true;
    } else
        return false;
}

void BinaryFile::Close()
{
    file.reset();
    filePath_.clear();
}

void BinaryFile::WriteSignedInt(int32_t i) const
{
    CHECKED_WRITE(libendian::le_write_i(i, *file));
}

void BinaryFile::WriteUnsignedInt(uint32_t i) const
{
    CHECKED_WRITE(libendian::le_write_ui(i, *file));
}

void BinaryFile::WriteSignedShort(int16_t i) const
{
    CHECKED_WRITE(libendian::le_write_s(i, *file));
}

void BinaryFile::WriteUnsignedShort(uint16_t i) const
{
    CHECKED_WRITE(libendian::le_write_us(i, *file));
}

void BinaryFile::WriteSignedChar(char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, *file));
}

void BinaryFile::WriteUnsignedChar(unsigned char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, *file));
}

void BinaryFile::WriteRawData(const void* const data, const unsigned length) const
{
    CHECKED_WRITE(libendian::write((const char*)data, length, *file));
}

void BinaryFile::WriteShortString(const std::string& str) const
{
    if(str.length() + 1 > std::numeric_limits<unsigned char>::max())
        throw std::out_of_range("String '" + str + "' is to long for a short string");
    auto length = static_cast<unsigned char>(str.length() + 1);
    WriteUnsignedChar(length);
    WriteRawData(str.c_str(), length);
}

void BinaryFile::WriteLongString(const std::string& str) const
{
    auto length = unsigned(str.length() + 1);
    WriteUnsignedInt(length);
    WriteRawData(str.c_str(), length);
}

int BinaryFile::ReadSignedInt()
{
    int32_t i;
    CHECKED_READ(libendian::le_read_i(&i, *file));
    return i;
}

unsigned BinaryFile::ReadUnsignedInt()
{
    uint32_t i;
    CHECKED_READ(libendian::le_read_ui(&i, *file));
    return i;
}

short BinaryFile::ReadSignedShort()
{
    int16_t i;
    CHECKED_READ(libendian::le_read_s(&i, *file));
    return i;
}

unsigned short BinaryFile::ReadUnsignedShort()
{
    uint16_t i;
    CHECKED_READ(libendian::le_read_us(&i, *file));
    return i;
}

char BinaryFile::ReadSignedChar()
{
    char i;
    CHECKED_READ(libendian::read(&i, 1, *file));
    return i;
}

unsigned char BinaryFile::ReadUnsignedChar()
{
    unsigned char i;
    CHECKED_READ(libendian::read(&i, 1, *file));
    return i;
}

void BinaryFile::ReadRawData(void* const data, const unsigned length)
{
    CHECKED_READ(libendian::read((char*)data, length, *file));
}

std::string BinaryFile::ReadShortString()
{
    unsigned char length;
    length = ReadUnsignedChar();
    auto* tmp = new char[length];
    ReadRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

std::string BinaryFile::ReadLongString()
{
    unsigned length;
    length = ReadUnsignedInt();
    auto* tmp = new char[length];
    ReadRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

void BinaryFile::Seek(const long pos, const int origin)
{
    fseek(*file, pos, origin); //-V303
}

long BinaryFile::Tell() const
{
    return ftell(*file); //-V303
}

void BinaryFile::Flush()
{
    fflush(*file);
}

bool BinaryFile::IsEndOfFile() const
{
    return feof(*file) != 0;
}
