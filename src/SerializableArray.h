// $Id: SerializableArray.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef SERIALIZABLEARRAY_H_INCLUDED
#define SERIALIZABLEARRAY_H_INCLUDED

#pragma once

#include "Message.h"

template <class Type, class Allocator = Type>
class SerializableArray
{
    public:
        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Konstruktor von @p SerializableArray.
         *
         *  @author FloSoft
         */
        SerializableArray(void)
        {
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Destruktor von @p SerializableArray.
         *
         *  @author FloSoft
         */
        ~SerializableArray(void)
        {
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Einfügefunktion
         *
         *  @author FloSoft
         */
        inline void push_back(const Type& item)
        {
            elements.push_back(item);
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Zuweisungsoperator.
         *
         *  @author FloSoft
         */
        template<class T, class A>
        SerializableArray& operator= (const SerializableArray<T, A> &other)
        {
            return copy(other);
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Zuweisungsoperator, spezifisch
         *
         *  @author FloSoft
         */
        SerializableArray& operator= (const SerializableArray& other)
        {
            return copy(other);
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Kopierfunktion
         *
         *  @author FloSoft
         */
        template<class T, class A>
        SerializableArray& copy(const SerializableArray<T, A> &other)
        {
            clear();

            elements.reserve(other.getCount());

            for(unsigned int i = 0; i < other.getCount(); ++i)
            {
                Type N = *other.getElement(i);
                elements.push_back(N);
            }

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  Array-Operatoren
         *
         *  @author FloSoft
         */
        inline Type&       operator[](unsigned int i)       { return elements.at(i); }
        inline const Type& operator[](unsigned int i) const { return elements.at(i); }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  räumt die Liste auf.
         *
         *  @author FloSoft
         */
        inline void clear(void)
        {
            elements.clear();
        }

        ///////////////////////////////////////////////////////////////////////////////
        /*
         *  serialisiert die Daten.
         *
         *  @param[in,out] data Datensatz, muss groß genug sein
         *
         *  @return liefert die Größe der Daten zurück.
         *
         *  @author FloSoft
         */
        inline void serialize(Message* msg) const
        {
            unsigned int count = elements.size();

            if(msg)
                msg->PushUnsignedInt(count);

            for(unsigned int i = 0; i < count; ++i)
                elements[i].serialize(msg);
        }

        ///////////////////////////////////////////////////////////////////////////////
        /*
         *  deserialisiert die Daten.
         *
         *  @param[in] data Datensatz, muss groß genug sein
         *
         *  @return liefert die Größe der gelesenen Daten zurück.
         *
         *  @author FloSoft
         */
        inline void deserialize(Message* msg)
        {
            if(!msg)
                return;

            unsigned int count = msg->PopUnsignedInt();

            clear();

            elements.reserve(count);

            for(unsigned int i = 0; i < count; ++i)
            {
                elements.push_back(Allocator(i, msg));
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  liefert ein Element der Liste.
         *
         *  @author FloSoft
         */
        inline const Type* getElement(unsigned int i) const
        {
            if(i < elements.size())
                return &elements.at(i);

            return NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  liefert ein Element der Liste.
         *
         *  @author FloSoft
         */
        inline Type* getElement(unsigned int i)
        {
            if(i < elements.size())
                return &elements.at(i);

            return NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  liefert die Anzahl der Elemente.
         *
         *  @author FloSoft
         */
        inline unsigned int getCount(void) const
        {
            return elements.size();
        }

        ///////////////////////////////////////////////////////////////////////////////
        /**
         *  ändert die Größe des Arrays
         *
         *  @author OLiver
         */
        void resize(const unsigned count)
        {
            return elements.resize(count);
        }


    private:
        std::vector<Type> elements;     ///< Die Elemente
};

#endif // SERIALIZABLEARRAY_H_INCLUDED
