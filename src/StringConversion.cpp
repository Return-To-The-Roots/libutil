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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "StringConversion.h"
#include <locale>

namespace s25util {

template<class T_Base>
ClassicImbuedStream<T_Base>::ClassicImbuedStream()
{
    this->imbue(std::locale::classic());
}
template<class T_Base>
ClassicImbuedStream<T_Base>::ClassicImbuedStream(const std::string& value) : T_Base(value)
{
    this->imbue(std::locale::classic());
}

// Instantiate with all sstream types
template struct ClassicImbuedStream<std::istringstream>;
template struct ClassicImbuedStream<std::ostringstream>;
template struct ClassicImbuedStream<std::stringstream>;
} // namespace s25util
