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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "BinaryFile.h"
#include <boost/array.hpp>
#include <boost/nowide/cstdio.hpp>
#include <stdexcept>

#define CHECKED_READ(...)  \
    if((__VA_ARGS__) != 0) \
    throw std::runtime_error("Unknown error during reading " + filePath)
#define CHECKED_WRITE(...) \
    if((__VA_ARGS__) != 0) \
    throw std::runtime_error("Unknown error during writing " + filePath)

bool BinaryFile::Open(const std::string& filePath, const OpenFileMode of)
{
    static const boost::array<const char*, 3> modes = {{"w+b", "a+b", "rb"}};
    file = bnw::fopen(filePath.c_str(), modes[of]);
    if(file)
    {
        this->filePath = filePath;
        return true;
    } else
        return false;
}

bool BinaryFile::Close()
{
    if(!file)
        return true;

    bool result = (fclose(file) == 0);
    file = NULL;
    filePath.clear();

    return result;
}

void BinaryFile::WriteSignedInt(int32_t i) const
{
    CHECKED_WRITE(libendian::le_write_i(i, file));
}

void BinaryFile::WriteUnsignedInt(uint32_t i) const
{
    CHECKED_WRITE(libendian::le_write_ui(i, file));
}

void BinaryFile::WriteSignedShort(int16_t i) const
{
    CHECKED_WRITE(libendian::le_write_s(i, file));
}

void BinaryFile::WriteUnsignedShort(uint16_t i) const
{
    CHECKED_WRITE(libendian::le_write_us(i, file));
}

void BinaryFile::WriteSignedChar(char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, file));
}

void BinaryFile::WriteUnsignedChar(unsigned char i) const
{
    CHECKED_WRITE(libendian::write(&i, 1, file));
}

void BinaryFile::WriteRawData(const void* const data, const unsigned length) const
{
    CHECKED_WRITE(libendian::write((char*)data, length, file));
}

void BinaryFile::WriteShortString(const std::string& str)
{
    if(str.length() + 1 > std::numeric_limits<unsigned char>::max())
        throw std::out_of_range("String '" + str + "' is to long for a short string");
    unsigned char length = static_cast<unsigned char>(str.length() + 1);
    WriteUnsignedChar(length);
    WriteRawData(str.c_str(), length);
}

void BinaryFile::WriteLongString(const std::string& str)
{
    unsigned length = unsigned(str.length() + 1);
    WriteUnsignedInt(length);
    WriteRawData(str.c_str(), length);
}

int BinaryFile::ReadSignedInt()
{
    int32_t i;
    CHECKED_READ(libendian::le_read_i(&i, file));
    return i;
}

unsigned BinaryFile::ReadUnsignedInt()
{
    uint32_t i;
    CHECKED_READ(libendian::le_read_ui(&i, file));
    return i;
}

short BinaryFile::ReadSignedShort()
{
    int16_t i;
    CHECKED_READ(libendian::le_read_s(&i, file));
    return i;
}

unsigned short BinaryFile::ReadUnsignedShort()
{
    uint16_t i;
    CHECKED_READ(libendian::le_read_us(&i, file));
    return i;
}

char BinaryFile::ReadSignedChar()
{
    char i;
    CHECKED_READ(libendian::read(&i, 1, file));
    return i;
}

unsigned char BinaryFile::ReadUnsignedChar()
{
    unsigned char i;
    CHECKED_READ(libendian::read(&i, 1, file));
    return i;
}

void BinaryFile::ReadRawData(void* const data, const unsigned length)
{
    CHECKED_READ(libendian::read((char*)data, length, file));
}

std::string BinaryFile::ReadShortString()
{
    unsigned char length;
    length = ReadUnsignedChar();
    char* tmp = new char[length];
    ReadRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

std::string BinaryFile::ReadLongString()
{
    unsigned length;
    length = ReadUnsignedInt();
    char* tmp = new char[length];
    ReadRawData(tmp, length);
    std::string str = tmp;
    delete[] tmp;
    return str;
}

void BinaryFile::Seek(const long pos, const int origin)
{
    fseek(file, pos, origin);
}

unsigned BinaryFile::Tell() const
{
    return ftell(file);
}

void BinaryFile::Flush()
{
    fflush(file);
}

bool BinaryFile::EndOfFile() const
{
    return feof(file) ? true : false;
}
