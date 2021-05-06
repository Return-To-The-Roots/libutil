# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

if(NOT MSVC)
    #Check for ccache
    if(APPLE)
        find_program(CCACHE_PROGRAM NAMES ccache-lipo ccache)
    else()
        find_program(CCACHE_PROGRAM ccache)
    endif()
    mark_as_advanced(CCACHE_PROGRAM)
    if(CCACHE_PROGRAM)
        foreach(lang C CXX)
            # Do only set ccache compiler if we don't already use a ccache wrapper
            # Example: /usr/lib/ccache/g++
            if(NOT CMAKE_${lang}_COMPILER MATCHES "ccache")
                message(STATUS "Using ccache(${CCACHE_PROGRAM}) for ${lang} to speed up builds")
                set(CMAKE_${lang}_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
            endif()
        endforeach()
    endif()
endif()
