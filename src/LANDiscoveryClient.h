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

#ifndef LANDISCOVERY_CLIENT_H_INCLUDED
#define LANDISCOVERY_CLIENT_H_INCLUDED

#include "LANDiscovery.h"
#include <string>
#include <map>

class LANDiscoveryClient: protected LANDiscoveryBase
{
public:
    struct ServiceInfo
    {
        std::string ip;
        Info info;
    };
    typedef std::map<std::string, ServiceInfo> ServiceMap;

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

#endif // !LANDISCOVERY_CLIENT_H_INCLUDED
