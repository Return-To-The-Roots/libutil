// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Socket.h"
#include <array>
#include <cstdint>
#include <string>

class LANDiscoveryBase
{
public:
    using Magic_t = std::array<char, 7>;
    struct Config
    {
        Magic_t magicQuery, magicResponse;
        uint8_t version;
        uint16_t portQuery, portResponse;

        static Magic_t makeMagic(const std::string& str);
    };

    struct Request
    {
        // Struct must be send over network with a fixed size
        std::array<char, sizeof(Magic_t) + sizeof(uint8_t)> data;
        Magic_t& getMagic() { return reinterpret_cast<Magic_t&>(data[0]); }
        uint8_t& getVersion() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t)]); }
    };
    struct Info
    {
        // Struct must be send over network with a fixed size
        using Payload = std::array<char, 192>;
        std::array<char, sizeof(Magic_t) + sizeof(uint8_t) * 2 + sizeof(Payload)> data;
        Magic_t& getMagic() { return reinterpret_cast<Magic_t&>(data[0]); }
        uint8_t& getVersion() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t)]); }
        uint8_t& getIsActive() { return reinterpret_cast<uint8_t&>(data[sizeof(Magic_t) + sizeof(uint8_t)]); } //-V119
        Payload& getPayload()
        {
            return reinterpret_cast<Payload&>(data[sizeof(Magic_t) + sizeof(uint8_t) * 2]);
        } //-V119
        const Payload& getPayload() const
        {
            return reinterpret_cast<const Payload&>(data[sizeof(Magic_t) + sizeof(uint8_t) * 2]);
        } //-V119
    };

    LANDiscoveryBase(const Config& cfg, bool isServer);

protected:
    /// Inits the socket and starts listening for broadcasts
    bool start();
    /// Closes the socket
    void stop();
    /// Returns true if any data is available to read
    bool isDataAvailable();
    /// Sends a broadcast
    void broadcast(const void* buffer, int len);

    const Config config;
    Socket socket;
    /// Request packet to send
    Request request;

private:
    bool isServer;
};
