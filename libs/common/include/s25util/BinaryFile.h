// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "libendian/libendian.h"
#include <boost/filesystem/path.hpp>
#include <cstdio>
#include <string>

// Öffnungsmethoden
enum OpenFileMode
{
    OFM_WRITE = 0,
    OFM_WRITE_ADD,
    OFM_READ
};

/// Klasse für das Laden und Speichern von binären Dateien
class BinaryFile
{
public:
    BinaryFile() : file(nullptr) {}
    ~BinaryFile() { Close(); }

    /// Öffnet eine Datei: liefert true falls erfolgreich
    bool Open(const boost::filesystem::path& filePath, OpenFileMode of);
    /// Schließt eine Datei: liefert true falls erfolgreich
    bool Close();

    /// Schreibmethoden
    void WriteSignedInt(int32_t i) const;
    void WriteUnsignedInt(uint32_t i) const;
    void WriteSignedShort(int16_t i) const;
    void WriteUnsignedShort(uint16_t i) const;
    void WriteSignedChar(char i) const;
    void WriteUnsignedChar(unsigned char i) const;
    void WriteRawData(const void* data, unsigned length) const;

    void WriteShortString(const std::string& str) const; /// Länge max 254
    void WriteLongString(const std::string& str) const;  /// Länge max 2^32-2

    /// Lesemethoden
    int ReadSignedInt();
    unsigned ReadUnsignedInt();
    short ReadSignedShort();
    unsigned short ReadUnsignedShort();
    char ReadSignedChar();
    unsigned char ReadUnsignedChar();
    void ReadRawData(void* data, unsigned length);

    std::string ReadShortString(); /// Länge max 254
    std::string ReadLongString();  /// Länge max 2^32-2

    /// Setzt den Dateizeiger
    void Seek(long pos, int origin);
    /// Liefert Den Dateizeiger
    unsigned Tell() const;
    /// Schreibt alles richtig in die Datei
    void Flush();
    /// Datei zu Ende?
    bool EndOfFile() const;
    /// Datei gültig?
    bool IsValid() const { return file ? true : false; }

    const boost::filesystem::path& getFilePath() const { return filePath_; }

private:
    /// File-Pointer
    FILE* file;
    boost::filesystem::path filePath_;
};
