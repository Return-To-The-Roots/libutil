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

#include "SingletonPolicies.h" // IWYU pragma: export

///////////////////////////////////////////////////////////////////////////////
/// Singleton-Template-class
/// \tparam T Type of the singleton
/// \tparam T_LifetimePolicy Policy that handles the lifetime of the singleton
template <typename T, template<class> class LifetimePolicy = SingletonPolicies::DefaultLifetime>
class Singleton
{
    protected:
        // protected ctor
        Singleton();
    private:
        Singleton(const Singleton&);
        Singleton& operator = (const Singleton&);
        static void MakeInstance();
        static void DestroySingleton();

    public:
        virtual ~Singleton();

        // Access to single instance
        inline static T& inst();

    private:
        static T* me;
        static bool isDestroyed_;
};

#include "SingletonImp.hpp"

#endif // SINGLETON_H_INCLUDED
