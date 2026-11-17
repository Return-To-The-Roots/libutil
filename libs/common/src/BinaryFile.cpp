// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "BinaryFile.h"
#include <boost/nowide/cstdio.hpp>
#include <array>
#include <cerrno>
#include <cstring>
#include <limits>
#include <stdexcept>

#define CHECKED_READ(...) \
    if(!(__VA_ARGS__))    \
    throw std::runtime_error("Unknown error during reading " + filePath_.string())
#define CHECKED_WRITE(...) \
    if(!(__VA_ARGS__))     \
    throw std::runtime_error("Unknown error during writing " + filePath_.string())

bool BinaryFile::open(const boost::filesystem::path& filePath, const OpenFileMode of)
{
    close();
    static constexpr std::array<const char*, 3> modes = {{"w+b", "a+b", "rb"}};
    file.reset(boost::nowide::fopen(filePath.string().c_str(), modes[static_cast<unsigned>(of)]));
    if(file)
    {
        filePath_ = filePath;
        return true;
    } else
        return false;
}

void BinaryFile::close()
{
    file.reset();
    filePath_.clear();
}

void BinaryFile::writeSignedInt(int32_t i) const
{
    CHECKED_WRITE(libendian::le_write_i(i, *file));
}

void BinaryFile::writeUnsignedInt(uint32_t i) const
{
    CHECKED_WRITE(libendian::le_write_ui(i, *file));
}

void BinaryFile::writeSignedShort(int16_t i) const
{
    CHECKED_WRITE(libendian::le_write_s(i, *file));
}

void BinaryFile::writeUnsignedShort(uint16_t i) const
{
    CHECKED_WRITE(libendian::le_write_us(i, *file));
}

void BinaryFile::writeSignedChar(char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, *file));
}

void BinaryFile::writeUnsignedChar(unsigned char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, *file));
}

void BinaryFile::writeRawData(const void* const data, const unsigned length) const
{
    CHECKED_WRITE(libendian::write((const char*)data, length, *file));
}

void BinaryFile::writeShortString(const std::string& str) const
{
    if(str.length() + 1 > std::numeric_limits<unsigned char>::max())
        throw std::out_of_range("String '" + str + "' is to long for a short string");
    auto length = static_cast<unsigned char>(str.length() + 1);
    writeUnsignedChar(length);
    writeRawData(str.c_str(), length);
}

void BinaryFile::writeLongString(const std::string& str) const
{
    auto length = unsigned(str.length() + 1);
    writeUnsignedInt(length);
    writeRawData(str.c_str(), length);
}

int BinaryFile::readSignedInt()
{
    int32_t i;
    CHECKED_READ(libendian::le_read_i(&i, *file));
    return i;
}

unsigned BinaryFile::readUnsignedInt()
{
    uint32_t i;
    CHECKED_READ(libendian::le_read_ui(&i, *file));
    return i;
}

short BinaryFile::readSignedShort()
{
    int16_t i;
    CHECKED_READ(libendian::le_read_s(&i, *file));
    return i;
}

unsigned short BinaryFile::readUnsignedShort()
{
    uint16_t i;
    CHECKED_READ(libendian::le_read_us(&i, *file));
    return i;
}

char BinaryFile::readSignedChar()
{
    char i;
    CHECKED_READ(libendian::read(&i, 1, *file));
    return i;
}

unsigned char BinaryFile::readUnsignedChar()
{
    unsigned char i;
    CHECKED_READ(libendian::read(&i, 1, *file));
    return i;
}

void BinaryFile::readRawData(void* const data, const unsigned length)
{
    CHECKED_READ(libendian::read((char*)data, length, *file));
}

std::string BinaryFile::readShortString()
{
    unsigned char length;
    length = readUnsignedChar();
    auto* tmp = new char[length];
    readRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

std::string BinaryFile::readLongString()
{
    unsigned length;
    length = readUnsignedInt();
    auto* tmp = new char[length];
    readRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

void BinaryFile::seek(const long pos, const int origin)
{
    fseek(*file, pos, origin); //-V303
}

unsigned long BinaryFile::tell() const
{
    const auto res = ftell(*file);
    if(res < 0)
        throw std::runtime_error("Failed to get file position: " + std::string(strerror(errno)));
    return static_cast<unsigned long>(res);
}

void BinaryFile::flush()
{
    fflush(*file);
}

bool BinaryFile::isEndOfFile() const
{
    return feof(*file) != 0;
}
