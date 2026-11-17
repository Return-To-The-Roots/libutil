// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscoveryClient.h"

LANDiscoveryClient::LANDiscoveryClient(const Config& cfg) : LANDiscoveryBase(cfg, false), isRunning(false) {}

LANDiscoveryClient::~LANDiscoveryClient() = default;

bool LANDiscoveryClient::start()
{
    if(isRunning)
        return true;

    if(!LANDiscoveryBase::start())
        return false;
    isRunning = true;

    refresh();
    return true;
}

void LANDiscoveryClient::stop()
{
    if(!isRunning)
        return;

    LANDiscoveryBase::stop();
    isRunning = false;
}

void LANDiscoveryClient::run()
{
    if(!isRunning)
        return;
    while(isDataAvailable())
    {
        ServiceInfo curInfo;
        PeerAddr addr;

        if(socket.recv(&curInfo.info, sizeof(curInfo.info), addr) < static_cast<int>(sizeof(curInfo.info)))
            break;
        if(curInfo.info.getMagic() != config.magicResponse || curInfo.info.getVersion() != config.version)
            continue;

        curInfo.ip = addr.getIp();
        if(curInfo.info.getIsActive())
            services[curInfo.ip] = curInfo;
        else
            services.erase(curInfo.ip);
    }
}

void LANDiscoveryClient::refresh()
{
    services.clear();
    broadcast(request.data.data(), sizeof(request));
}
