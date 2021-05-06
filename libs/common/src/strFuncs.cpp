// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
// Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "strFuncs.h"
#include <algorithm>
#include <cstring>
#include <random>
#include <stdexcept>
#include <utility>

namespace detail {

void strcpyExt(char* pOut, const char* pSrc, const size_t maxChars)
{
    size_t len = strlen(pSrc) + 1;
    if(len < maxChars)
        std::copy_n(pSrc, len, pOut);
    else
        throw std::range_error("Cannot copy string. Destination buffer to small.");
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
    std::mt19937 rng;
    std::uniform_int_distribution<std::string::size_type> distr;
    explicit RandCharCreator(const std::string& charset)
        : charset(charset), rng(std::random_device()()), distr(0, charset.length())
    {}
    RandCharCreator(const std::string& charset, uint32_t seed) : charset(charset), rng(seed), distr(0, charset.length())
    {}
    char operator()() { return charset[distr(rng)]; }
};

namespace {
std::string createRandString(size_t len, RandCharCreator creator) //-V813
{
    std::string result;
    result.resize(len);
    std::generate_n(result.begin(), len, std::move(creator));
    return result;
}
} // namespace

std::string createRandString(size_t len, const std::string& charset)
{
    return createRandString(len, RandCharCreator(charset));
}

std::string createRandString(size_t len, const std::string& charset, uint32_t seed)
{
    return createRandString(len, RandCharCreator(charset, seed));
}
