// Copyright (c) 2019 - 2019 Settlers Freaks (sf-team at siedler25.org)
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
