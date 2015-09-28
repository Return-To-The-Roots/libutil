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
#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#pragma once

#include <stdexcept>
#include <cstdlib>

/// Policy that throws an exception on a dead ref
struct OnDeadRefThrow
{
    template<typename T>
    static void
    onDeadRef(T)
    {
        throw std::runtime_error("Access to dead singleton detected!");
    }
};

/// Policy that recreates the singleton on dead ref (Phoenix singleton)
struct OnDeadRefRecreate
{
    template<typename T>
    static void
    onDeadRef(T* inst)
    {
        // Placement new
        new(inst) T();
        atexit(&T::KillMe);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// Singleton-Template-class
/// \tparam T Type of the singleton
/// \tparam T_OnDeadRef Policy that handles dead references
template <typename T, class T_OnDeadRef = OnDeadRefThrow>
class Singleton
{
    typedef T_OnDeadRef OnDeadRef;

    protected:
        // protected ctor
        Singleton() {
            // Init invalid access detection
            isDestroyed_ = false;
        }
    private:
        Singleton(const Singleton&);
        Singleton& operator = (const Singleton&);

    public:
        virtual ~Singleton()
        {
            isDestroyed_ = true;
        }

        // Used for manually destroying the singleton
        // Do not use from user code!
        static void KillMe()
        {
            me.~Singleton();
        }

        // Access to single instance
        inline static T& inst() {
            // Note: We rely on the fact that on the first call to this function
            // "me" must have been constructed and therefore isDestroyed_ is set
            if(isDestroyed_)
                OnDeadRef::onDeadRef(&me);
            return me;
        }

    private:
        static T me;
        static bool isDestroyed_;
};

///////////////////////////////////////////////////////////////////////////////
// Template-Fix ?!?
#if _MSCVER <= 710
template<typename T, class T_OnDeadRef> T Singleton<T, T_OnDeadRef>::me;
template<typename T, class T_OnDeadRef> bool Singleton<T, T_OnDeadRef>::isDestroyed_;
#endif // _MSCVER > 710

#endif // SINGLETON_H_INCLUDED
