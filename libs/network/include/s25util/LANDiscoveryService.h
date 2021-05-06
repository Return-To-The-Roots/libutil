// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "LANDiscovery.h"

class LANDiscoveryService : protected LANDiscoveryBase
{
    Info info;
    bool isRunning, dataChanged;

public:
    LANDiscoveryService(const Config& cfg);
    ~LANDiscoveryService();
    /// Sets the payload data to send
    void SetPayload(const void* data, size_t len);
    /// Initializes the service so it can be run
    bool Start();
    /// Closes the service sending a final close packet if it was running
    void Stop();
    /// Must be called in the main loop when the service is started. No-Op if it isn't
    void Run();
};
