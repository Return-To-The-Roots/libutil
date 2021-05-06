// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "IP.hpp"
#include "s25util/Tokenizer.h"
#include <stdexcept>

namespace {
uint8_t parseByte(const std::string& s)
{
    // 0-255 only
    if(s.empty() || s.length() > 3)
        throw std::invalid_argument("Invalid input");
    uint16_t result = 0;
    for(const auto c : s)
    {
        if(c >= '0' && c <= '9')
            result = result * 10 + (c - '0');
        else
            throw std::invalid_argument("Invalid char");
    }
    if(result > 255)
        throw std::invalid_argument("Out of range");
    return static_cast<uint8_t>(result);
}
} // namespace

namespace s25util {
uint32_t stringToIP(const std::string& ipStr)
{
    Tokenizer tokenizer(ipStr, ".");
    uint32_t result = 0;
    int numParts = 0;
    try
    {
        while(tokenizer)
        {
            result = (result << 8) + parseByte(tokenizer.next());
            numParts++;
        }
    } catch(const std::exception&)
    {
        throw std::invalid_argument("Not a valid IP: " + ipStr);
    }
    if(numParts != 4)
    {
        throw std::invalid_argument("Not a valid IP or incomplete: " + ipStr);
    }
    return result;
}
} // namespace s25util
