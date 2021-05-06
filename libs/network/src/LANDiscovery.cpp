// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscovery.h"
#include "SocketSet.h"
#include <algorithm>
#include <stdexcept>

LANDiscoveryBase::Magic_t LANDiscoveryBase::Config::MakeMagic(const std::string& str)
{
    if(str.empty())
        throw std::invalid_argument("String is empty");
    if(str.length() > sizeof(Magic_t))
        throw std::invalid_argument("String is to long");
    Magic_t result;
    std::fill(result.begin(), result.end(), 0);
    std::copy(str.begin(), str.end(), result.begin());
    return result;
}

LANDiscoveryBase::LANDiscoveryBase(const Config& cfg, bool isServer) : config(cfg), isServer(isServer)
{
    request.GetMagic() = cfg.magicQuery;
    request.GetVersion() = cfg.version;
}

bool LANDiscoveryBase::Start()
{
    if(!socket.Create(AF_INET, true))
        return false;

    return socket.Bind(isServer ? config.portQuery : config.portResponse, false);
}

void LANDiscoveryBase::Stop()
{
    socket.Close();
}

bool LANDiscoveryBase::IsDataAvailable()
{
    SocketSet set;
    set.Add(socket);
    return set.Select(0, 0) == 1;
}

void LANDiscoveryBase::Broadcast(const void* const buffer, const int len)
{
    PeerAddr addr(!isServer ? config.portQuery : config.portResponse);
    socket.Send(buffer, len, addr);
}
