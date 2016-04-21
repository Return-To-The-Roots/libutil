// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace detail{

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
