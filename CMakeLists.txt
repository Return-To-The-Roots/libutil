cmake_minimum_required(VERSION 3.8)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
project(s25util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(external/nowide EXCLUDE_FROM_ALL)

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
