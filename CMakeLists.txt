# Copyright (C) 2005 - 2025 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.16..3.20)
project(s25util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
if(CMAKE_VERSION VERSION_LESS 3.14)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_3.14")
endif()

# The super project might e.g. call find_package(Boost ... nowide), if the target is not defined, download directly
if(NOT TARGET Boost::nowide)
    message(STATUS "Fetching Boost.Nowide from repository...")
    include(FetchContent)
    FetchContent_Declare(
      BoostNowide
      GIT_REPOSITORY https://github.com/boostorg/nowide.git
      GIT_TAG        v11.2.0
    )
    include(SaveVariables)
    save_variables(BUILD_SHARED_LIBS CMAKE_POSITION_INDEPENDENT_CODE BUILD_TESTING)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(BUILD_TESTING OFF)
    FetchContent_MakeAvailable(BoostNowide)
    pop_saved_variables()
endif()

add_subdirectory(libs)
if(ClangFormat_FOUND)
    add_ClangFormat_folder(libs TRUE)
endif()

include(RttrTestingCfg)
if(BUILD_TESTING)
    add_subdirectory(tests)
    if(ClangFormat_FOUND)
        add_ClangFormat_folder(tests TRUE)
    endif()
endif()
