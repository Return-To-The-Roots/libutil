// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <vector>

/// Split a string at given delimiter chars
class Tokenizer
{
public:
    /**
     *  Ctor of @p Tokenizer.
     *
     *  @param[in] data      Data to be separated
     *  @param[in] delimiter Chars to cut the data
     */
    explicit Tokenizer(std::string data, std::string delimiter = "; \t");

    /**
     *  Get the state of the Tokenizer
     *
     *  @return @p true If next() will return data
     *          @p false if all data has been used
     */
    operator bool() const;

    /**
     *  Extract next token
     *
     *  @return The next token
     */
    std::string next();

    /// Convert the (remaining) data to a vector
    std::vector<std::string> explode();

private:
    /// The string
    std::string data;
    /// Delimiters
    std::string delimiter;
    size_t curPos;
};
