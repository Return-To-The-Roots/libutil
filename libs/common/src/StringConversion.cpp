// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "StringConversion.h"
#include <iomanip>
#include <locale>

namespace s25util::detail {
void imbueClassic(std::ios& stream)
{
    stream.imbue(std::locale::classic());
}
void enableHexOutput(std::ostream& s, size_t typeSize)
{
    s << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(typeSize * 2);
}

} // namespace s25util::detail
