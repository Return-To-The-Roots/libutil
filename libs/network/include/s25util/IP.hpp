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

#ifndef IP_h_INCLUDED__
#define IP_h_INCLUDED__

#include <cstdint>
#include <string>

namespace s25util {
constexpr uint32_t makeIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return (a << 24) + (b << 16) + (c << 8) + d;
}
uint32_t stringToIP(const std::string&);
} // namespace s25util

#endif // IP_h_INCLUDED__