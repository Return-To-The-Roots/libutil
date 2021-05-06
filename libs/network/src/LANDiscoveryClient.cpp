// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscoveryClient.h"

LANDiscoveryClient::LANDiscoveryClient(const Config& cfg) : LANDiscoveryBase(cfg, false), isRunning(false) {}

LANDiscoveryClient::~LANDiscoveryClient() = default;

bool LANDiscoveryClient::Start()
{
    if(isRunning)
        return true;

    if(!LANDiscoveryBase::Start())
        return false;
    isRunning = true;

    Refresh();
    return true;
}

void LANDiscoveryClient::Stop()
{
    if(!isRunning)
        return;

    LANDiscoveryBase::Stop();
    isRunning = false;
}

void LANDiscoveryClient::Run()
{
    if(!isRunning)
        return;
    while(IsDataAvailable())
    {
        ServiceInfo curInfo;
        PeerAddr addr;

        if(socket.Recv(&curInfo.info, sizeof(curInfo.info), addr) < static_cast<int>(sizeof(curInfo.info)))
            break;
        if(curInfo.info.GetMagic() != config.magicResponse || curInfo.info.GetVersion() != config.version)
            continue;

        curInfo.ip = addr.GetIp();
        if(curInfo.info.GetIsActive())
            services[curInfo.ip] = curInfo;
        else
            services.erase(curInfo.ip);
    }
}

void LANDiscoveryClient::Refresh()
{
    services.clear();
    Broadcast(&request.data[0], sizeof(request));
}
