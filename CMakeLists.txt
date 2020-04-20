cmake_minimum_required(VERSION 3.8)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
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
    GIT_TAG        v10.0.1
  )
  include(SaveVariables)
  save_variables(BUILD_SHARED_LIBS CMAKE_POSITION_INDEPENDENT_CODE)
  set(BUILD_SHARED_LIBS OFF)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
