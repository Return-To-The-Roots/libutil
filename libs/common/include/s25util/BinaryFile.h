// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "file_handle.h"
#include "libendian/libendian.h"
#include <boost/filesystem/path.hpp>
#include <cstdio>
#include <string>

enum class OpenFileMode : unsigned
{
    Write,
    WriteAppend,
    Read,
};

/// Class for reading and writing binary files with fixed endianness
class BinaryFile
{
public:
    /// open the file and return true on success
    bool open(const boost::filesystem::path& filePath, OpenFileMode of);
    /// close the file
    void close();

    void writeSignedInt(int32_t i) const;
    void writeUnsignedInt(uint32_t i) const;
    void writeSignedShort(int16_t i) const;
    void writeUnsignedShort(uint16_t i) const;
    void writeSignedChar(char i) const;
    void writeUnsignedChar(unsigned char i) const;
    void writeRawData(const void* data, unsigned length) const;

    void writeShortString(const std::string& str) const; /// max length 254
    void writeLongString(const std::string& str) const;  /// max length 2^32-2

    int readSignedInt();
    unsigned readUnsignedInt();
    short readSignedShort();
    unsigned short readUnsignedShort();
    char readSignedChar();
    unsigned char readUnsignedChar();
    void readRawData(void* data, unsigned length);

    std::string readShortString(); /// max length 254
    std::string readLongString();  /// max length 2^32-2

    /// Move the file position
    void seek(long pos, int origin);
    /// Get file position, throws on error
    unsigned long tell() const;
    /// flush buffer to disk
    void flush();
    bool isEndOfFile() const;
    /// Is file opened
    bool isOpen() const { return static_cast<bool>(file); }

    const boost::filesystem::path& getFilePath() const { return filePath_; }

private:
    s25util::file_handle file;
    boost::filesystem::path filePath_;
};
