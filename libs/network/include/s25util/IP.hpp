// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <string>

namespace s25util {
constexpr uint32_t makeIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return (a << 24) + (b << 16) + (c << 8) + d;
}
uint32_t stringToIP(const std::string&);
} // namespace s25util
