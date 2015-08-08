// $Id: Serializer.h 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#pragma once

#include "libUtilDefines.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif // _WIN32

#include "uvector.h"
#include <string>
#include <algorithm>

class BinaryFile;

/// Klasse die einen Buffer zum Serialisieren verwaltet und entsprechende Methoden zum Lesen/Schreiben bereitstellt.
/// Implementiert einen FIFO (push fügt ans ende, pop entfernt am anfang)
class Serializer
{
    public:
        Serializer(void)
            : data(0), length(0), pos(0)
        {
        }

        Serializer(const void* const data, const unsigned initial_size)
            : data(initial_size), length(0), pos(0)
        {
            PushRawData(data, initial_size);
        }

        virtual ~Serializer()
        {
            Clear();
        }

        /// Aufräumen
        inline void Clear()
        {
            data.clear();
            length = 0;
            pos = 0;
        }

        unsigned GetPos() const
        {
            return pos;
        }

        /// Getter
        unsigned GetLength() const
        {
            return length;
        }

        /// Zugriff auf die Rohdaten
        const unsigned char* GetData(void) const
        {
            return data.data();
        }

        /// Schreibt den Buffer in eine Datei
        void WriteToFile(BinaryFile& file);
        /// Liest den Buffer aus einer Datei
        virtual void ReadFromFile(BinaryFile& file);


        /// Kopiermethoden

        /// Rohdaten kopieren
        inline void PushRawData(const void* const data, const unsigned length)
        {
            ExtendMemory(length);
            const char* const bData = reinterpret_cast<const char* const>(data);
            std::copy(bData, bData + length, this->data.begin() + this->length);
            this->length += length;
        }



        /// Sämtliche Integer
        inline void PushSignedInt(signed int i)
        {
            Push(i);
        }
        inline void PushUnsignedInt(unsigned int i)
        {
            Push(i);
        }

        inline void PushSignedShort(signed short int i)
        {
            Push(i);
        }
        inline void PushUnsignedShort(unsigned short int i)
        {
            Push(i);
        }

        inline void PushSignedChar(signed char i)
        {
            Push(i);
        }
        inline void PushUnsignedChar(unsigned char i)
        {
            Push(i);
        }

        inline void PushBool(bool b)
        {
            PushUnsignedChar(b ? 1 : 0);
        }

        inline void PushString(const std::string& str)
        {
            PushUnsignedInt(str.length());
            for(unsigned i = 0; i < str.length(); ++i)
                PushSignedChar(str[i]);
        }

        // Lesemethoden

        /// Rohdaten kopieren
        inline void PopRawData(void* const data, const unsigned length)
        {
            assert(pos + length <= this->length);

            char* const bData = reinterpret_cast<char* const>(data);
            std::copy(this->data.begin() + pos, this->data.begin() + pos + length, bData);
            pos += length;
        }

        /// Sämtliche Integer
        inline signed int PopSignedInt()
        {
            return Pop<signed int>();
        }
        inline unsigned int PopUnsignedInt()
        {
            return Pop<unsigned int>();
        }

        inline signed short PopSignedShort()
        {
            return Pop<signed short>();
        }
        inline unsigned short PopUnsignedShort()
        {
            return Pop<unsigned short>();
        }

        inline signed char PopSignedChar()
        {
            return Pop<signed char>();
        }

        inline unsigned char PopUnsignedChar()
        {
            return Pop<unsigned char>();
        }

        inline bool PopBool()
        {
            return ((PopUnsignedChar() == 1) ? true : false);
        }

        inline std::string PopString()
        {
            std::string str;
            str.resize(PopUnsignedInt());

            assert(pos + str.size() <= length);

            for(unsigned i = 0; i < str.length(); ++i)
                str[i] = PopSignedChar();

            return str;
        }

    protected:
        Serializer& operator=(const Serializer& other)
        {
            if(this == &other)
                return *this;

            data = other.data;
            length = other.length;
            pos = other.pos;

            return *this;
        }

        unsigned int checkByteOrder(unsigned int i)
        {
            return htonl(i);
        }

        signed int checkByteOrder(signed int i)
        {
            return htonl(i);
        }

        unsigned short checkByteOrder(unsigned short i)
        {
            return htons(i);
        }

        signed short checkByteOrder(signed short i)
        {
            return htons(i);
        }

        unsigned char checkByteOrder(unsigned char i)
        {
            return i;
        }

        signed char checkByteOrder(signed char i)
        {
            return i;
        }

        template<typename T>
        T Pop()
        {
            assert(pos + sizeof(T) <= length);

            T i = checkByteOrder( *reinterpret_cast<T*>(&data[pos]) );
            pos += sizeof(T);

            return i;
        }

        template<typename T>
        void Push(const T i)
        {
            ExtendMemory(sizeof(T));
            *reinterpret_cast<T*>(&data[length]) = checkByteOrder(i);
            this->length += sizeof(T);
        }

        /// Schreibzugriff auf die Rohdaten
        unsigned char* GetDataWritable(void)
        {
            return data.data();
        }

        /// Schreibzugriff auf die Länge
        void SetLength(const unsigned int length)
        {
            this->length = length;
        }

        /// Erweitert ggf. Speicher um add_length
        inline void ExtendMemory(const unsigned add_length)
        {
            EnsureSize(length + add_length);
        }

        /// Makes sure the internal buffer is at least length bytes long
        inline void EnsureSize(const unsigned length)
        {
            data.reserve(length);
            data.resize(data.capacity());
        }

    private:
        /// data mit den Daten
        uvector<unsigned char> data;
        /// Logische Länge
        unsigned length;
        /// Schreib/Leseposition
        unsigned pos;
};

#endif // !SERIALIZER_H_INCLUDED
