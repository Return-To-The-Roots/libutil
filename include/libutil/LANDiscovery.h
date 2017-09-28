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

#ifndef LANDISCOVERY_H_INCLUDED
#define LANDISCOVERY_H_INCLUDED

#include "Socket.h"
#include <boost/array.hpp>
#include <stdint.h>
#include <string>

class LANDiscoveryBase
{
public:
    typedef boost::array<char, 7> Magic_t;
    struct Config
    {
        Magic_t magicQuery, magicResponse;
        uint8_t version;
        uint16_t portQuery, portResponse;

        static Magic_t MakeMagic(const std::string& str);
    };

    struct Request
    {
        // Struct must be send over network with a fixed size
        boost::array<char, sizeof(Magic_t) + sizeof(uint8_t)> data;
        Magic_t& GetMagic() { return reinterpret_cast<Magic_t&>(data[0]); }
        uint8_t& GetVersion() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t)]); }
    };
    struct Info
    {
        // Struct must be send over network with a fixed size
        typedef boost::array<char, 192> Payload;
        boost::array<char, sizeof(Magic_t) + sizeof(uint8_t) * 2 + sizeof(Payload)> data;
        Magic_t& GetMagic() { return reinterpret_cast<Magic_t&>(data[0]); }
        uint8_t& GetVersion() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t)]); }
        uint8_t& GetIsActive() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t) + sizeof(uint8_t)]); }                      //-V119
        Payload& GetPayload() { return reinterpret_cast<Payload&>(data[sizeof(Magic_t) + sizeof(uint8_t) * 2]); }                   //-V119
        const Payload& GetPayload() const { return reinterpret_cast<const Payload&>(data[sizeof(Magic_t) + sizeof(uint8_t) * 2]); } //-V119
    };

    LANDiscoveryBase(const Config& cfg, bool isServer);

protected:
    /// Inits the socket and starts listening for broadcasts
    bool Start();
    /// Closes the socket
    void Stop();
    /// Returns true if any data is available to read
    bool IsDataAvailable();
    /// Sends a broadcast
    void Broadcast(const void* const buffer, const int len);

    const Config config;
    Socket socket;
    /// Request packet to send
    Request request;

private:
    bool isServer;
};

#endif // !LANDISCOVERY_H_INCLUDED
