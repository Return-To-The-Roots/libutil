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
#include "LANDiscoveryClient.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif // _WIN32

// Include last!
#include "DebugNew.h"

LANDiscoveryClient::LANDiscoveryClient(const Config& cfg) : LANDiscoveryBase(cfg, false), isRunning(false)
{}

LANDiscoveryClient::~LANDiscoveryClient()
{}

bool LANDiscoveryClient::Start()
{
    if (isRunning)
        return true;

    if (!LANDiscoveryBase::Start())
        return false;
    isRunning = true;

    Refresh();
    return true;
}

void LANDiscoveryClient::Stop()
{
    if (!isRunning)
        return;

    LANDiscoveryBase::Stop();
    isRunning = false;
}

void LANDiscoveryClient::Run()
{
    if (!isRunning)
        return;
    while (IsDataAvailable())
    {
        ServiceInfo curInfo;
        PeerAddr addr;

        if (socket.Recv(&curInfo.info, sizeof(curInfo.info), addr) < static_cast<int>(sizeof(curInfo.info)))
            break;
        if (curInfo.info.GetMagic() != config.magicResponse || curInfo.info.GetVersion() != config.version)
            continue;

        curInfo.ip = addr.GetIp();
        if (curInfo.info.GetIsActive())
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