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

#ifndef SingletonPolicies_h__
#define SingletonPolicies_h__

#include <cstdlib>
#include <stdexcept>

namespace SingletonPolicies {

/// Type of the pointer to the destruction function
typedef void (*DestructionFunPtr)();

/// Policy that throws an exception on a dead ref
template<typename T>
struct DefaultLifetime
{
    static void OnDeadReference() { throw std::runtime_error("Access to dead singleton detected!"); }

    static void ScheduleDestruction(T*, DestructionFunPtr pFun) { std::atexit(pFun); }
};

/// Policy that recreates the singleton on dead ref (Phoenix singleton)
template<typename T>
struct Phoenix
{
    static bool isDestroyedOnce;

    static void OnDeadReference() { isDestroyedOnce = true; }

    static void ScheduleDestruction(T*, DestructionFunPtr pFun)
    {
        if(!isDestroyedOnce)
            std::atexit(pFun);
    }
};

template<typename T>
bool Phoenix<T>::isDestroyedOnce;

/// Sets the longevity for a given destruction function
/// Used for singletons. The dtor will be called with increasing longevity
void SetLongevity(unsigned longevity, DestructionFunPtr pFun);

/// Default implementation: Gets the longevity from the class constant Longevity
template<typename T>
unsigned GetLongevity(T*)
{
    return T::Longevity;
}

/// Policy that orders the destruction of singletons
/// Assumes a visible function GetLongevity(T*) that returns the longevity of the object
template<typename T>
struct WithLongevity
{
    static void OnDeadReference() { throw std::runtime_error("Access to dead singleton detected!"); }

    static void ScheduleDestruction(T* inst, DestructionFunPtr pFun) { SetLongevity(GetLongevity(inst), pFun); }
};

} // namespace SingletonPolicies

#endif // SingletonPolicies_h__
