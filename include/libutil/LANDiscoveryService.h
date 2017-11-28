// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef LANDISCOVERY_SERVICE_H_INCLUDED
#define LANDISCOVERY_SERVICE_H_INCLUDED

#include "LANDiscovery.h"

class LANDiscoveryService : protected LANDiscoveryBase
{
    Info info;
    bool isRunning, dataChanged;

public:
    LANDiscoveryService(const Config& cfg);
    ~LANDiscoveryService();
    /// Sets the payload data to send
    void SetPayload(const void* const data, size_t len);
    /// Initializes the service so it can be run
    bool Start();
    /// Closes the service sending a final close packet if it was running
    void Stop();
    /// Must be called in the main loop when the service is started. No-Op if it isn't
    void Run();
};

#endif // !LANDISCOVERY_SERVICE_H_INCLUDED
