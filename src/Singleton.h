// $Id: Singleton.h 9359 2014-04-25 15:37:22Z FloSoft $
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
#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Singleton-Template-Klasse
template <typename T>
class Singleton
{
    protected:
        // Instanz anlegen verboten
        Singleton() { }
        Singleton(const Singleton&);
        Singleton& operator = (const Singleton&);

    public:
        // Globaler Zugriff auf einzige Instanz
        inline static T& inst() {   return me; }

    private:
        static T me;
};

///////////////////////////////////////////////////////////////////////////////
// Template-Fix ?!?
#if _MSCVER <= 710
template<typename T> T Singleton<T>::me;
#endif // _MSCVER > 710

#endif // SINGLETON_H_INCLUDED
