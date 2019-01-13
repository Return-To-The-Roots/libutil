cmake_minimum_required(VERSION 3.8)
project(s25util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_library(utf8 INTERFACE)
target_include_directories(utf8 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(NOWIDE_BUILD_STATIC ON CACHE INTERNAL "")
set(NOWIDE_BUILD_SHARED OFF CACHE INTERNAL "")
set(NOWIDE_USE_BOOST_FILESYSTEM ON CACHE INTERNAL "")
add_subdirectory(nowide EXCLUDE_FROM_ALL)

add_subdirectory(src)
if(ClangFormat_FOUND)
    add_ClangFormat_folder(src TRUE)
endif()
    
include(RttrTestingCfg)
if(BUILD_TESTING)
    add_subdirectory(tests)
    if(ClangFormat_FOUND)
        add_ClangFormat_folder(tests TRUE)
    endif()
endif()
