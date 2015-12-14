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


#include "libUtilDefines.h"
#include "LANDiscoveryService.h"
#include <algorithm>
#include <exception>

LANDiscoveryService::LANDiscoveryService(const Config& cfg): LANDiscoveryBase(cfg, true), isRunning(false), dataChanged(false)
{
    info.GetMagic() = cfg.magicResponse;
    info.GetVersion() = cfg.version;
    info.GetIsActive() = false;
}

LANDiscoveryService::~LANDiscoveryService()
{
    Stop();
}

void LANDiscoveryService::SetPayload(const void* const data, size_t len)
{
    if (len > info.GetPayload().size())
        throw std::invalid_argument("Payload size is to big");
    const char* const buffer = static_cast<const char* const>(data);
    std::copy(buffer, buffer + std::min(len, info.GetPayload().size()), info.GetPayload().begin());
    info.GetIsActive() = true;
    dataChanged = true;
}

bool LANDiscoveryService::Start()
{
    if (isRunning)
        return true;
    if (!LANDiscoveryBase::Start())
        return false;
    isRunning = true;
    return true;
}

void LANDiscoveryService::Stop()
{
    if (!isRunning)
        return;

    if (info.GetIsActive())
    {
        // Tell clients we are offline
        info.GetIsActive() = false;
        Broadcast(&info, sizeof(info));
    }
    dataChanged = false;
    isRunning = false;
    LANDiscoveryBase::Stop();
}

void LANDiscoveryService::Run()
{
    if (!isRunning)
        return;

    if (dataChanged)
    {
        dataChanged = false;
        Broadcast(&info, sizeof(info));
    }
    while (IsDataAvailable())
    {
        Request curReq;
        PeerAddr addr;

        if (socket.Recv(&curReq, sizeof(curReq), addr) < sizeof(curReq))
            break;
        if (curReq.GetMagic() == request.GetMagic() && curReq.GetVersion() == request.GetVersion())
            socket.Send(&info, sizeof(info), addr);
    }
}