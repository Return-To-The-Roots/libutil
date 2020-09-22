// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
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

#include "Tokenizer.h"
#include <utility>

Tokenizer::Tokenizer(std::string data, std::string delimiter)
    : data(std::move(data)), delimiter(std::move(delimiter)), curPos(0)
{}

Tokenizer::operator bool() const
{
    return curPos != std::string::npos;
}

std::string Tokenizer::next()
{
    if(!*this)
        return "";

    const auto newPos = data.find_first_of(delimiter, curPos);
    auto result = data.substr(curPos, newPos - curPos);

    curPos = (newPos == std::string::npos) ? newPos : newPos + 1;
    return result;
}

std::vector<std::string> Tokenizer::explode()
{
    std::vector<std::string> result;
    while(*this)
        result.push_back(next());
    return result;
}
