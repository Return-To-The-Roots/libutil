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

#ifndef ProxySettings_h__
#define ProxySettings_h__

#include <string>

enum ProxyType
{
    PROXY_NONE = 0,
    PROXY_SOCKS4 = 4,
    PROXY_SOCKS5 = 5
};
struct ProxySettings
{
    ProxySettings() : type(PROXY_NONE), port(0) {}
    ProxySettings(ProxyType type, const std::string& hostname, unsigned port) : type(type), hostname(hostname), port(port) {}
    ProxyType type;
    std::string hostname;
    unsigned port;
};

#endif // ProxySettings_h__
