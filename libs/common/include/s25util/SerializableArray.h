// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Serializer.h"
#include <algorithm>
#include <vector>

template<typename Type>
class SerializableArray
{
    using Storage = std::vector<Type>;

public:
    using iterator = typename Storage::iterator;
    using const_iterator = typename Storage::const_iterator;

    SerializableArray() = default;

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
    void push_back(const Type& item) { elements.push_back(item); }

    Type& operator[](unsigned i) { return elements.at(i); }
    const Type& operator[](unsigned i) const { return elements.at(i); }

    /**
     *  räumt die Liste auf.
     *
     */
    void clear() { elements.clear(); }

    /**
     *  serialisiert die Daten.
     *
     *  @param[in,out] data Datensatz, muss groß genug sein
     *
     *  @return liefert die Größe der Daten zurück.
     *
     */
    void serialize(Serializer& ser) const
    {
        ser.PushUnsignedInt(static_cast<unsigned>(elements.size()));

        for(const auto& el : elements)
            el.serialize(ser);
    }

    /**
     *  deserialisiert die Daten.
     *
     *  @param[in] data Datensatz, muss groß genug sein
     *
     *  @return liefert die Größe der gelesenen Daten zurück.
     *
     */
    void deserialize(Serializer& ser)
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
    const Type* getElement(unsigned i) const
    {
        if(i < elements.size())
            return &elements.at(i);

        return nullptr;
    }

    /**
     *  liefert ein Element der Liste.
     *
     */
    Type* getElement(unsigned i)
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
    const_iterator begin() const noexcept { return elements.begin(); }
    iterator end() { return elements.end(); }
    const_iterator end() const noexcept { return elements.end(); }

private:
    std::vector<Type> elements; /// Die Elemente
};
