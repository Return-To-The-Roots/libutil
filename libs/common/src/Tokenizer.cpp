// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
