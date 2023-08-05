# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Boost 1.69 REQUIRED COMPONENTS filesystem)

file(GLOB HEADER_FILES include/s25util/*.*)
add_library(s25util_log STATIC
	${HEADER_FILES}
	src/error.cpp
	src/FileWriter.cpp
	src/FormatedLogEntry.cpp
	src/Log.cpp
	src/StdStreamWriter.cpp
	src/StringStreamWriter.cpp
)
add_library(s25util::log ALIAS s25util_log)

target_include_directories(s25util_log PUBLIC include PRIVATE include/s25util)
target_link_libraries(s25util_log PUBLIC s25util::common Boost::nowide Boost::disable_autolinking Boost::filesystem)
target_compile_features(s25util_log PUBLIC cxx_std_17)
# PIC Required for linking into shared libs
set_target_properties(s25util_log PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)

IF (WIN32)
    target_compile_definitions(s25util_log PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600) # Win Vista
    # Exclude some windows defines
    target_compile_definitions(s25util_log PRIVATE NOMINMAX NODRAWTEXT NOSOUND NOTEXTMETRIC NOCOMM NOMCX)
ENDIF ()

include(EnableWarnings)
enable_warnings(s25util_log)
