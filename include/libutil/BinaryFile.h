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
#ifndef BINARYFILE_H_INCLUDED
#define BINARYFILE_H_INCLUDED

#pragma once

#include "libendian/libendian.h"
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
    BinaryFile() : file(NULL) {}
    ~BinaryFile() { Close(); }

    /// Öffnet eine Datei: liefert true falls erfolgreich
    bool Open(const std::string& filePath, const OpenFileMode of);
    /// Schließt eine Datei: liefert true falls erfolgreich
    bool Close();

    /// Schreibmethoden
    void WriteSignedInt(int32_t i) const;
    void WriteUnsignedInt(uint32_t i) const;
    void WriteSignedShort(int16_t i) const;
    void WriteUnsignedShort(uint16_t i) const;
    void WriteSignedChar(char i) const;
    void WriteUnsignedChar(unsigned char i) const;
    void WriteRawData(const void* const data, const unsigned length) const;

    void WriteShortString(const std::string& str); /// Länge max 254
    void WriteLongString(const std::string& str);  /// Länge max 2^32-2

    /// Lesemethoden
    int ReadSignedInt();
    unsigned ReadUnsignedInt();
    short ReadSignedShort();
    unsigned short ReadUnsignedShort();
    char ReadSignedChar();
    unsigned char ReadUnsignedChar();
    void ReadRawData(void* const data, const unsigned length);

    std::string ReadShortString(); /// Länge max 254
    std::string ReadLongString();  /// Länge max 2^32-2

    /// Setzt den Dateizeiger
    void Seek(const long pos, const int origin);
    /// Liefert Den Dateizeiger
    unsigned Tell() const;
    /// Schreibt alles richtig in die Datei
    void Flush();
    /// Datei zu Ende?
    bool EndOfFile() const;
    /// Datei gültig?
    bool IsValid() const { return file ? true : false; }

    const std::string& getFilePath() const { return filePath; }

private:
    /// File-Pointer
    FILE* file;
    std::string filePath;
};

#endif // !BINARYFILE_H_INCLUDED
