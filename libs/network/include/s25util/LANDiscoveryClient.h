// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "LANDiscovery.h"
#include <map>
#include <string>

class LANDiscoveryClient : protected LANDiscoveryBase
{
public:
    struct ServiceInfo
    {
        std::string ip;
        Info info;
    };
    using ServiceMap = std::map<std::string, ServiceInfo>;

    LANDiscoveryClient(const Config& cfg);
    ~LANDiscoveryClient();
    /// Starts listening for services
    bool Start();
    /// Stops listing for services
    void Stop();
    /// Must be called in the main loop when this is listening. No-Op if it isn't
    void Run();
    /// Refreshes the service list, clearing the old one
    void Refresh();

    const ServiceMap& GetServices() const { return services; }

private:
    bool isRunning;
    ServiceMap services;
};
