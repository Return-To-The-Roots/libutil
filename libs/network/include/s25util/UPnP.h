// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

class UPnP
{
public:
    UPnP(uint16_t port);
    ~UPnP();

    void open(uint16_t port);
    void close();

    static void OpenPort(uint16_t port);
    static void ClosePort(uint16_t port);

private:
    uint16_t port_ = 0;
};
