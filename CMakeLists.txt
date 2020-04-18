cmake_minimum_required(VERSION 3.8)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
project(s25util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
if(CMAKE_VERSION VERSION_LESS 3.14)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_3.14")
endif()

option(RTTR_USE_SYSTEM_NOWIDE OFF "Use system installed Boost.Nowide. Fails if not found!")

if(Boost_VERSION_MINOR GREATER_EQUAL "73" OR RTTR_USE_SYSTEM_NOWIDE)
  # Boost 1.73 contains Boost.Nowide. So if we already found Boost or want the system nowide, use it
  find_package(Boost COMPONENTS nowide)
endif()
# Prefer system version, either as part of Boost or preinstalled
if(NOT Boost_NOWIDE_FOUND)
  if(RTTR_USE_SYSTEM_NOWIDE)
    set(bnw_required REQUIRED)
  else()
    set(bnw_required QUIET)
  endif()
  find_package(boost_nowide 10.0.0 ${bnw_required})
  if(NOT boost_nowide_FOUND)
    message(STATUS "No system-wide installed Boost.Nowide found. Fetching from repository...")
    include(FetchContent)
    FetchContent_Declare(
      BoostNowide
      GIT_REPOSITORY https://github.com/boostorg/nowide.git
      GIT_TAG        v10.0.0
    )
    set(_oldBUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_MakeAvailable(BoostNowide)
    set(BUILD_SHARED_LIBS ${_oldBUILD_SHARED_LIBS})
  endif()
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
