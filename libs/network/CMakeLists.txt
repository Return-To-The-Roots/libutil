# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

if(NOT MSVC)
    find_package(Miniupnpc REQUIRED)
    include(GatherDll)
    gather_dll(MINIUPNPC)
endif()

file(GLOB SRC_FILES src/*.*)
file(GLOB HEADER_FILES include/s25util/*.*)

conditional_src(SRC_FILES MSVC src/UPnP_MSVC.cpp src/UPnP_Other.cpp)

add_library(s25util_network STATIC ${SRC_FILES} ${HEADER_FILES})
add_library(s25util::network ALIAS s25util_network)

target_include_directories(s25util_network PUBLIC include PRIVATE include/s25util)
target_link_libraries(s25util_network PRIVATE s25util::common s25util::log)
target_compile_features(s25util_network PUBLIC cxx_std_17)
# PIC Required for linking into shared libs
set_target_properties(s25util_network PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    target_compile_definitions(s25util_network PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_link_libraries(s25util_network PRIVATE ${MINIUPNPC_LIBRARY})
    target_include_directories(s25util_network SYSTEM PRIVATE ${MINIUPNPC_INCLUDE_DIR})
ENDIF()

IF (WIN32)
    target_compile_definitions(s25util_network PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600) # Win Vista
    # Exclude some windows defines
    target_compile_definitions(s25util_network PRIVATE NOMINMAX NODRAWTEXT NOSOUND NOTEXTMETRIC NOCOMM NOMCX)
    target_link_libraries(s25util_network PUBLIC ws2_32)
ENDIF ()

include(EnableWarnings)
enable_warnings(s25util_network)
