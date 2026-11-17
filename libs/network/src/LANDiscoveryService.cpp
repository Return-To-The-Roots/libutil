// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscoveryService.h"
#include <algorithm>
#include <stdexcept>

LANDiscoveryService::LANDiscoveryService(const Config& cfg)
    : LANDiscoveryBase(cfg, true), isRunning(false), dataChanged(false)
{
    info.getMagic() = cfg.magicResponse;
    info.getVersion() = cfg.version;
    info.getIsActive() = 0;
}

LANDiscoveryService::~LANDiscoveryService()
{
    stop();
}

void LANDiscoveryService::setPayload(const void* const data, size_t len)
{
    if(len > info.getPayload().size()) //-V807
        throw std::invalid_argument("Payload size is to big");
    const auto* const buffer = static_cast<const char*>(data);
    std::copy(buffer, buffer + std::min(len, info.getPayload().size()), info.getPayload().begin());
    info.getIsActive() = 1;
    dataChanged = true;
}

bool LANDiscoveryService::start()
{
    if(isRunning)
        return true;
    if(!LANDiscoveryBase::start())
        return false;
    isRunning = true;
    return true;
}

void LANDiscoveryService::stop()
{
    if(!isRunning)
        return;

    if(info.getIsActive())
    {
        // Tell clients we are offline
        info.getIsActive() = 0;
        broadcast(&info, sizeof(info));
    }
    dataChanged = false;
    isRunning = false;
    LANDiscoveryBase::stop();
}

void LANDiscoveryService::run()
{
    if(!isRunning)
        return;

    if(dataChanged)
    {
        dataChanged = false;
        broadcast(&info, sizeof(info));
    }
    while(isDataAvailable())
    {
        Request curReq;
        PeerAddr addr;

        if(socket.recv(&curReq, sizeof(curReq), addr) < static_cast<int>(sizeof(curReq)))
            break;
        if(curReq.getMagic() == request.getMagic() && curReq.getVersion() == request.getVersion())
            socket.send(&info, sizeof(info), addr);
    }
}
