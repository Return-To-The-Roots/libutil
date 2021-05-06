// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "UPnP.h"

UPnP::UPnP(uint16_t port)
{
    open(port);
}

UPnP::~UPnP()
{
    try
    {
        close();
    } catch(...)
    {}
}

void UPnP::open(uint16_t port)
{
    close();
    OpenPort(port);
    port_ = port;
}

void UPnP::close()
{
    if(port_)
    {
        ClosePort(port_);
        port_ = 0;
    }
}
