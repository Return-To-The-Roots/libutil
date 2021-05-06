// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdlib>
#include <stdexcept>

namespace SingletonPolicies {

/// Type of the pointer to the destruction function
using DestructionFunPtr = void (*)();

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
