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
    openPort(port);
    port_ = port;
}

void UPnP::close()
{
    if(port_)
    {
        closePort(port_);
        port_ = 0;
    }
}
