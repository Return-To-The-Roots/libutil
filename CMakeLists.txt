CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
# CMake 2.8.9 required for POSITION_INDEPENDENT_CODE target property

add_library(utf8 INTERFACE)
target_include_directories(utf8 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
ADD_SUBDIRECTORY( src ) 
