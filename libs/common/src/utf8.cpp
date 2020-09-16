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

#include "utf8.h"
#include <boost/nowide/detail/convert.hpp>
#include <boost/nowide/detail/utf.hpp>
#include <array>
#include <cstdint>

namespace utf = boost::nowide::detail::utf;

namespace s25util {

std::u32string utf8to32(const std::string& other)
{
    return boost::nowide::detail::convert_string<char32_t>(other.data(), other.data() + other.size());
}

std::string utf32to8(const std::u32string& other)
{
    return boost::nowide::detail::convert_string<char>(other.data(), other.data() + other.size());
}

bool isValidUTF8(const std::string& text)
{
    return findInvalidUTF8(text) == text.end();
}

std::string::const_iterator findInvalidUTF8(const std::string& text)
{
    for(auto it = text.begin(), end = text.end(); it != end;)
    {
        const auto itCur = it;
        const auto c = utf::utf_traits<char>::decode(it, end);
        if(c == utf::illegal || c == utf::incomplete)
            return itCur;
    }
    return text.end();
}

namespace {
    constexpr std::array<uint16_t, 0x80> highAnsiToUnicode = {
      0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152,
      0x008D, 0x017D, 0x008F, 0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122,
      0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178, 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6,
      0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3,
      0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 0x00C0,
      0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD,
      0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA,
      0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
      0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4,
      0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF};
}

std::string ansiToUTF8(const std::string& other)
{
    if(isValidUTF8(other))
        return other;

    std::string result;
    result.reserve(other.length());
    for(const char c : other)
    {
        const auto uc = static_cast<uint8_t>(c); // Make unsigned
        const utf::code_point cp = (uc < 0x80) ? uc : highAnsiToUnicode[uc - 0x80];
        utf::utf_traits<char>::encode(cp, std::back_inserter(result));
    }
    return result;
}

} // namespace s25util
