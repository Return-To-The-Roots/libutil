# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# VS does not support coverage analysis
if(NOT MSVC)
    option(RTTR_ENABLE_COVERAGE "Generate coverage build" OFF)
    if(RTTR_ENABLE_COVERAGE)
        if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            message(FATAL_ERROR "Coverage requires a debug build or you get false positives")
        endif()
        # Note: "--coverage" instead of "-coverage" makes this work with ccache
        add_compile_options(-O0 --coverage)
        string(APPEND CMAKE_EXE_LINKER_FLAGS " --coverage")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " --coverage")
        if(CMAKE_COMPILER_IS_GNUCXX)
            # Inlining makes the coverage statistic much harder to read and may lead to lots of partials
            # However expect a slowdown!
            message(STATUS "Enabled no-inline for better coverage statistics")
            add_compile_options(-fno-inline -fno-inline-small-functions -fno-default-inline)
        else()
            message(STATUS "Using default inlining which probably influences coverage statistics")
        endif()
    endif()
endif()
