// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <string>
#include <utility>

enum class ProxyType
{
    None = 0,
    Socks4 = 4,
    Socks5 = 5
};
struct ProxySettings
{
    ProxySettings() : type(ProxyType::None), port(0) {}
    ProxySettings(ProxyType type, std::string hostname, uint16_t port)
        : type(type), hostname(std::move(hostname)), port(port)
    {}
    ProxyType type;
    std::string hostname;
    uint16_t port;
};
