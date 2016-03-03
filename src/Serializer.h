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

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
#endif // _WIN32

#include "uvector.h"
#include <string>
#include <algorithm>
#include <stdexcept>

class BinaryFile;

/// Klasse die einen Buffer zum Serialisieren verwaltet und entsprechende Methoden zum Lesen/Schreiben bereitstellt.
/// Implementiert einen FIFO (push fügt ans ende, pop entfernt am anfang)
class Serializer
{
    public:
        Serializer()
            : data_(0), length_(0), pos_(0)
        {
        }

        Serializer(const void* const data, const unsigned initial_size)
            : data_(initial_size), length_(0), pos_(0)
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
            data_.clear();
            length_ = 0;
            pos_ = 0;
        }

        unsigned GetPos() const
        {
            return pos_;
        }

        /// Getter
        unsigned GetLength() const
        {
            return length_;
        }

        /// Schreibzugriff auf die Länge
        void SetLength(const unsigned int length)
        {
            EnsureSize(length);
            this->length_ = length;
        }

        unsigned GetBytesLeft() const
        {
            assert(pos_ < length_);
            return length_ - pos_;
        }

        /// Zugriff auf die Rohdaten
        const unsigned char* GetData() const
        {
            return data_.data();
        }

        /// Schreibzugriff auf die Rohdaten
        unsigned char* GetDataWritable(unsigned length)
        {
            EnsureSize(length);
            return data_.data();
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
            std::copy(bData, bData + length, this->data_.begin() + this->length_);
            this->length_ += length;
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
            PushUnsignedInt(static_cast<unsigned>(str.length()));
            for(unsigned i = 0; i < str.length(); ++i)
                PushSignedChar(str[i]);
        }

        // Lesemethoden

        /// Rohdaten kopieren
        inline void PopRawData(void* const data, const unsigned length)
        {
            CheckSize(length);

            char* const bData = reinterpret_cast<char* const>(data);
            std::copy(this->data_.begin() + pos_, this->data_.begin() + pos_ + length, bData);
            pos_ += length;
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
            unsigned char value = PopUnsignedChar();
            assert(value == 0 || value == 1);
            return ( (value != 0) ? true : false);
        }

        inline std::string PopString()
        {
            std::string str;
            str.resize(PopUnsignedInt());

            CheckSize(static_cast<unsigned>(str.size()));

            for(unsigned i = 0; i < str.length(); ++i)
                str[i] = PopSignedChar();

            return str;
        }

    protected:
        Serializer(const Serializer& other): data_(other.data_), length_(other.length_), pos_(other.pos_)
        {}

        Serializer& operator=(const Serializer& other)
        {
            if(this == &other)
                return *this;

            data_ = other.data_;
            length_ = other.length_;
            pos_ = other.pos_;

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
            CheckSize(sizeof(T));

            T i = checkByteOrder( *reinterpret_cast<T*>(&data_[pos_]) );
            pos_ += sizeof(T);

            return i;
        }

        template<typename T>
        void Push(const T i)
        {
            ExtendMemory(sizeof(T));
            *reinterpret_cast<T*>(&data_[length_]) = checkByteOrder(i);
            this->length_ += sizeof(T);
        }

        /// Erweitert ggf. Speicher um add_length
        inline void ExtendMemory(const unsigned add_length)
        {
            EnsureSize(length_ + add_length);
        }

        /// Makes sure the internal buffer is at least length bytes long
        inline void EnsureSize(const unsigned length)
        {
            if(data_.size() < length)
            {
                data_.reserve(length);
                data_.resize(data_.capacity());
            }
        }

    private:
        /// data mit den Daten
        uvector<unsigned char> data_;
        /// Logische Länge
        unsigned length_;
        /// Schreib/Leseposition
        unsigned pos_;

        /// Checks if data of size len can be popped
        void CheckSize(const unsigned len)
        {
            if(pos_ + len > length_)
                throw std::range_error("Out of range during deserialization");
        }
};

#endif // !SERIALIZER_H_INCLUDED
