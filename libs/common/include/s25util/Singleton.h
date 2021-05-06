// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SingletonPolicies.h" // IWYU pragma: export

///////////////////////////////////////////////////////////////////////////////
/// Singleton-Template-class
/// \tparam T Type of the singleton
/// \tparam T_LifetimePolicy Policy that handles the lifetime of the singleton
template<typename T, template<class> class LifetimePolicy = SingletonPolicies::DefaultLifetime>
class Singleton
{
protected:
    // protected ctor
    Singleton();
    ~Singleton();

private:
    static void MakeInstance();
    static void DestroySingleton();
    static bool& getIsDestroyed();

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // Access to single instance
    inline static T& inst();

private:
    static T* me;
};

#include "SingletonImp.hpp"
