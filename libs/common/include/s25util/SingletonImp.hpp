// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cassert>

// IWYU pragma: private, include "Singleton.h"

template<typename T, template<class> class L>
T* Singleton<T, L>::me;

template<typename T, template<class> class L>
bool& Singleton<T, L>::getIsDestroyed()
{
    static bool is_destroyed = false;
    return is_destroyed;
}

template<typename T, template<class> class L>
Singleton<T, L>::Singleton()
{
    // Init invalid access detection
    getIsDestroyed() = false;
}

template<typename T, template<class> class L>
Singleton<T, L>::~Singleton()
{
    getIsDestroyed() = true;
}

template<typename T, template<class> class LifetimePolicy>
void Singleton<T, LifetimePolicy>::MakeInstance()
{
    if(me)
        return;
    if(getIsDestroyed())
    {
        LifetimePolicy<T>::OnDeadReference();
        getIsDestroyed() = false;
    }
    me = new T;
    LifetimePolicy<T>::ScheduleDestruction(me, &DestroySingleton);
}

template<typename T, template<class> class L>
void Singleton<T, L>::DestroySingleton()
{
    assert(!getIsDestroyed());
    delete me;
    me = nullptr;
    getIsDestroyed() = true;
}

template<typename T, template<class> class L>
T& Singleton<T, L>::inst()
{
    if(!me)
        MakeInstance();
    return *me; // NOLINT
}
