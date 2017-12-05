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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "strFuncs.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace detail {

void strcpyExt_check(char* pOut, const char* pSrc, const size_t maxChars)
{
    size_t len = strlen(pSrc) + 1;
    if(len < maxChars)
        memcpy(pOut, pSrc, len);
    else
        throw std::range_error("Cannot copy string. Destination buffer to small.");
}

void strcpyExt_trunc(char* pOut, const char* pSrc, const size_t maxChars)
{
    if(maxChars == 0)
        throw std::invalid_argument("Destination buffer size must be > 0");

    size_t len = std::min(strlen(pSrc), maxChars - 1);
    memcpy(pOut, pSrc, len);
    pOut[len] = '0';
}

} // namespace detail

std::string createRandString(size_t len, bool useLowercase, bool useUppercase, bool useNumbers, bool useSpecialChars)
{
    std::string charset;
    if(useLowercase)
        charset += "abcdefghijklmnopqrstuvwxyz";
    if(useUppercase)
        charset += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(useNumbers)
        charset += "0123456789";
    if(useSpecialChars)
        charset += "!@#$%^&*()`~-_=+[{]{\\|;:'\",<.>/? ";
    return createRandString(len, charset);
}

struct RandCharCreator
{
    const std::string charset;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<std::string::size_type> distr;
    explicit RandCharCreator(const std::string& charset)
        : charset(charset), rng(boost::random::random_device()()), distr(0, charset.length())
    {}
    char operator()() { return charset[distr(rng)]; }
};

std::string createRandString(size_t len, const std::string& charset)
{
    std::string result;
    result.resize(len);
    std::generate_n(result.begin(), len, RandCharCreator(charset));
    return result;
}
