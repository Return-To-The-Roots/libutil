// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscoveryService.h"
#include <algorithm>
#include <stdexcept>

LANDiscoveryService::LANDiscoveryService(const Config& cfg)
    : LANDiscoveryBase(cfg, true), isRunning(false), dataChanged(false)
{
    info.GetMagic() = cfg.magicResponse;
    info.GetVersion() = cfg.version;
    info.GetIsActive() = 0;
}

LANDiscoveryService::~LANDiscoveryService()
{
    Stop();
}

void LANDiscoveryService::SetPayload(const void* const data, size_t len)
{
    if(len > info.GetPayload().size()) //-V807
        throw std::invalid_argument("Payload size is to big");
    const auto* const buffer = static_cast<const char*>(data);
    std::copy(buffer, buffer + std::min(len, info.GetPayload().size()), info.GetPayload().begin());
    info.GetIsActive() = 1;
    dataChanged = true;
}

bool LANDiscoveryService::Start()
{
    if(isRunning)
        return true;
    if(!LANDiscoveryBase::Start())
        return false;
    isRunning = true;
    return true;
}

void LANDiscoveryService::Stop()
{
    if(!isRunning)
        return;

    if(info.GetIsActive())
    {
        // Tell clients we are offline
        info.GetIsActive() = 0;
        Broadcast(&info, sizeof(info));
    }
    dataChanged = false;
    isRunning = false;
    LANDiscoveryBase::Stop();
}

void LANDiscoveryService::Run()
{
    if(!isRunning)
        return;

    if(dataChanged)
    {
        dataChanged = false;
        Broadcast(&info, sizeof(info));
    }
    while(IsDataAvailable())
    {
        Request curReq;
        PeerAddr addr;

        if(socket.Recv(&curReq, sizeof(curReq), addr) < static_cast<int>(sizeof(curReq)))
            break;
        if(curReq.GetMagic() == request.GetMagic() && curReq.GetVersion() == request.GetVersion())
            socket.Send(&info, sizeof(info), addr);
    }
}
