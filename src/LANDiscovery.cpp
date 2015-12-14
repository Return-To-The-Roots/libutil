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
#include "LANDiscovery.h"
#include "SocketSet.h"
#include <algorithm>

LANDiscoveryBase::Magic_t LANDiscoveryBase::Config::MakeMagic(const std::string& str)
{
    if (str.empty())
        throw std::invalid_argument("String is empty");
    if(str.length() > sizeof(Magic_t))
        throw std::invalid_argument("String is to long");
    Magic_t result;
    std::fill(result.begin(), result.end(), 0);
    std::copy(str.begin(), str.end(), result.begin());
    return result;
}

LANDiscoveryBase::LANDiscoveryBase(const Config& cfg, bool isServer): config(cfg), isServer(isServer)
{
    request.GetMagic() = cfg.magicQuery;
    request.GetVersion() = cfg.version;
}

bool LANDiscoveryBase::Start()
{
    if (!socket.Create(AF_INET, true))
        return false;

    return socket.Bind(isServer ? config.portQuery : config.portResponse, false);
}

void LANDiscoveryBase::Stop()
{
    socket.Close();
}

bool LANDiscoveryBase::IsDataAvailable() {
    SocketSet set;
    set.Add(socket);
    return set.Select(0, 0) == 1;
}

void LANDiscoveryBase::Broadcast(const void* const buffer, const int len)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port = htons(!isServer ? config.portQuery : config.portResponse);
    socket.Send(buffer, len, addr);
}