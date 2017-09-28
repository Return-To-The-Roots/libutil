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
        void WriteSignedInt(int i) const
        {
            libendian::le_write_i(i, file);
        }
        void WriteUnsignedInt(unsigned i) const
        {
            libendian::le_write_ui(i, file);
        }
        void WriteSignedShort(short i) const
        {
            libendian::le_write_s(i, file);
        }

        void WriteUnsignedShort(unsigned short i) const
        {
            libendian::le_write_us(i, file);
        }
        void WriteSignedChar(char i) const
        {
            libendian::le_write_c(&i, 1, file);
        }
        void WriteUnsignedChar(unsigned char i) const
        {
            libendian::le_write_uc(&i, 1, file);
        }
        void WriteRawData(const void* const data, const unsigned length) const
        {
            libendian::le_write_c((char*)data, length, file);
        }

        void WriteShortString(const std::string& str); /// Länge max 254
        void WriteLongString(const std::string& str); /// Länge max 2^32-2

        /// Lesemethoden
        int ReadSignedInt()
        {
            signed int i;
            libendian::le_read_i(&i, file);
            return i;
        }
        unsigned ReadUnsignedInt()
        {
            unsigned i;
            libendian::le_read_ui(&i, file);
            return i;
        }
        short ReadSignedShort()
        {
            signed short i;
            libendian::le_read_s(&i, file);
            return i;
        }
        unsigned short ReadUnsignedShort()
        {
            unsigned short i;
            libendian::le_read_us(&i, file);
            return i;
        }
        char ReadSignedChar()
        {
            char i;
            libendian::le_read_c(&i, 1, file);
            return i;
        }
        unsigned char ReadUnsignedChar()
        {
            unsigned char i;
            libendian::le_read_uc(&i, 1, file);
            return i;
        }
        void ReadRawData(void* const data, const unsigned length)
        {
            libendian::le_read_c((char*)data, length, file);
        }

        std::string ReadShortString(); /// Länge max 254
        std::string ReadLongString(); /// Länge max 2^32-2

        /// Setzt den Dateizeiger
        void Seek(const long pos, const int origin)
        {
            fseek(file, pos, origin);
        }

        /// Liefert Den Dateizeiger
        unsigned Tell() const
        {
            return ftell(file);
        }

        /// Schreibt alles richtig in die Datei
        void Flush()
        {
            fflush(file);
        }

        /// Datei zu Ende?
        bool EndOfFile() const
        {
            return feof(file) ? true : false;
        }

        /// Datei gültig?
        bool IsValid() const
        {
            return file ? true : false;
        }

        const std::string& getFilePath() const
        {
            return filePath;
        }

    private:
        /// File-Pointer
        FILE* file;
        std::string filePath;
};


#endif // !BINARYFILE_H_INCLUDED