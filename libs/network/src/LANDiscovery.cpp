// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LANDiscovery.h"
#include "SocketSet.h"
#include <algorithm>
#include <stdexcept>

LANDiscoveryBase::Magic_t LANDiscoveryBase::Config::makeMagic(const std::string& str)
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
    request.getMagic() = cfg.magicQuery;
    request.getVersion() = cfg.version;
}

bool LANDiscoveryBase::start()
{
    if(!socket.create(AF_INET, true))
        return false;

    return socket.bind(isServer ? config.portQuery : config.portResponse, false);
}

void LANDiscoveryBase::stop()
{
    socket.close();
}

bool LANDiscoveryBase::isDataAvailable()
{
    SocketSet set;
    set.add(socket);
    return set.select(0, 0) == 1;
}

void LANDiscoveryBase::broadcast(const void* const buffer, const int len)
{
    PeerAddr addr(!isServer ? config.portQuery : config.portResponse);
    socket.send(buffer, len, addr);
}
