cmake_minimum_required(VERSION 3.8)

add_library(utf8 INTERFACE)
target_include_directories(utf8 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(NOWIDE_BUILD_STATIC ON CACHE INTERNAL "")
set(NOWIDE_BUILD_SHARED OFF CACHE INTERNAL "")
set(NOWIDE_USE_BOOST_FILESYSTEM ON CACHE INTERNAL "")
add_subdirectory(nowide/src EXCLUDE_FROM_ALL)

if(NOT RTTR_NO_LIBUTIL)
	add_subdirectory(src)
endif()
