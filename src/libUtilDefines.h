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
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#pragma once
#ifndef LIB_UTIL_DEFINES_H__
#define LIB_UTIL_DEFINES_H__

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#ifdef _WIN32
#   ifndef _CRTDBG_MAP_ALLOC
#       define _CRTDBG_MAP_ALLOC
#   endif
#   define WIN32_LEAN_AND_MEAN

    // We need at least the win vista version for all required defines
#   if defined _WIN32_WINNT && _WIN32_WINNT < 0x600
        //#undef _WIN32_WINNT
#   endif
#   ifndef _WIN32_WINNT
        //#define _WIN32_WINNT 0x600
#   endif

#   ifdef _MSC_VER
#       include <crtdbg.h>
#       define getch _getch
#       if !defined(snprintf) && _MSC_VER < 1800
#           define snprintf _snprintf
#       endif
#       ifndef assert
#           define assert _ASSERT
#       endif
#   else
#       include <assert.h>
#   endif

#   ifdef _DEBUG
#       include <crtdbg.h>
#   endif // _DEBUG
#else //_WIN32
#   include <assert.h>
#endif //_WIN32

#endif
