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
    /// Open the file and return true on success
    bool Open(const boost::filesystem::path& filePath, OpenFileMode of);
    /// Close the file
    void Close();

    void WriteSignedInt(int32_t i) const;
    void WriteUnsignedInt(uint32_t i) const;
    void WriteSignedShort(int16_t i) const;
    void WriteUnsignedShort(uint16_t i) const;
    void WriteSignedChar(char i) const;
    void WriteUnsignedChar(unsigned char i) const;
    void WriteRawData(const void* data, unsigned length) const;

    void WriteShortString(const std::string& str) const; /// max length 254
    void WriteLongString(const std::string& str) const;  /// max length 2^32-2

    int ReadSignedInt();
    unsigned ReadUnsignedInt();
    short ReadSignedShort();
    unsigned short ReadUnsignedShort();
    char ReadSignedChar();
    unsigned char ReadUnsignedChar();
    void ReadRawData(void* data, unsigned length);

    std::string ReadShortString(); /// max length 254
    std::string ReadLongString();  /// max length 2^32-2

    /// Move the file position
    void Seek(long pos, int origin);
    /// Get file position, throws on error
    unsigned long Tell() const;
    /// Flush buffer to disk
    void Flush();
    bool IsEndOfFile() const;
    /// Is file opened
    bool IsOpen() const { return static_cast<bool>(file); }

    const boost::filesystem::path& getFilePath() const { return filePath_; }

private:
    s25util::file_handle file;
    boost::filesystem::path filePath_;
};
