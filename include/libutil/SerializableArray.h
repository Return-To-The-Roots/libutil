// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "Serializer.h"
#include <algorithm>
#include <vector>

template<typename Type>
class SerializableArray
{
    typedef std::vector<Type> Storage;

public:
    typedef typename Storage::iterator iterator;
    typedef typename Storage::const_iterator const_iterator;

    SerializableArray() {}

    template<typename T>
    friend class SerializableArray;

    template<class T>
    SerializableArray(const SerializableArray<T>& other)
    {
        elements.reserve(other.size());
        std::copy(other.elements.begin(), other.elements.end(), std::back_inserter(elements));
    }

    template<class T>
    SerializableArray& operator=(const SerializableArray<T>& other)
    {
        if(this == &other)
            return *this;

        elements.clear();
        elements.reserve(other.size());
        std::copy(other.elements.begin(), other.elements.end(), std::back_inserter(elements));

        return *this;
    }

    /**
     *  Einfügefunktion
     *
     */
    inline void push_back(const Type& item) { elements.push_back(item); }

    inline Type& operator[](unsigned i) { return elements.at(i); }
    inline const Type& operator[](unsigned i) const { return elements.at(i); }

    /**
     *  räumt die Liste auf.
     *
     */
    inline void clear() { elements.clear(); }

    /**
     *  serialisiert die Daten.
     *
     *  @param[in,out] data Datensatz, muss groß genug sein
     *
     *  @return liefert die Größe der Daten zurück.
     *
     */
    inline void serialize(Serializer& ser) const
    {
        ser.PushUnsignedInt(static_cast<unsigned>(elements.size()));

        for(typename std::vector<Type>::const_iterator it = elements.begin(); it != elements.end(); ++it)
            it->serialize(ser);
    }

    /**
     *  deserialisiert die Daten.
     *
     *  @param[in] data Datensatz, muss groß genug sein
     *
     *  @return liefert die Größe der gelesenen Daten zurück.
     *
     */
    inline void deserialize(Serializer& ser)
    {
        clear();
        unsigned count = ser.PopUnsignedInt();
        elements.reserve(count);

        for(unsigned i = 0; i < count; ++i)
            elements.push_back(Type(i, ser));
    }

    /**
     *  liefert ein Element der Liste.
     *
     */
    inline const Type* getElement(unsigned i) const
    {
        if(i < elements.size())
            return &elements.at(i);

        return nullptr;
    }

    /**
     *  liefert ein Element der Liste.
     *
     */
    inline Type* getElement(unsigned i)
    {
        if(i < elements.size())
            return &elements.at(i);

        return nullptr;
    }

    /**
     *  liefert die Anzahl der Elemente.
     *
     */
    unsigned size() const { return static_cast<unsigned>(elements.size()); }
    bool empty() const { return elements.empty(); }

    /**
     *  ändert die Größe des Arrays
     *
     */
    void resize(const unsigned count) { return elements.resize(count); }

    iterator begin() { return elements.begin(); }
    const_iterator begin() const { return elements.begin(); }
    iterator end() { return elements.end(); }
    const_iterator end() const { return elements.end(); }

private:
    std::vector<Type> elements; /// Die Elemente
};

#endif // SERIALIZABLEARRAY_H_INCLUDED
