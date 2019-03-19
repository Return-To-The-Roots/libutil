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

#ifndef TOKENIZER_HPP_INCLUDED
#define TOKENIZER_HPP_INCLUDED

#pragma once

#include <string>
#include <vector>

/// Tokenizer-Klasse.
class Tokenizer
{
public:
    /**
     *  Ctor of @p Tokenizer.
     *
     *  @param[in] data      Data to be separated
     *  @param[in] delimiter Chars to cut the data
     *
     *  @author FloSoft
     */
    explicit Tokenizer(std::string data, std::string delimiter = "; \t");

    /**
     *  Get the state of the Tokenizer
     *
     *  @return @p true If next() will return data
     *          @p false if all data has been used
     *
     *  @author FloSoft
     */
    operator bool() const;

    /**
     *  Extract next token
     *
     *  @return The next token
     *
     *  @author FloSoft
     */
    std::string next();

    /// Convert the (remaining) data to a vector
    std::vector<std::string> explode();

private:
    /// The internal (remaining) string
    std::string data;
    /// Delimiters
    std::string delimiter;
};

#endif // !TOKENIZER_HPP_INCLUDED
