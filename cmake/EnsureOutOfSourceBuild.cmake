# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# Ensures that the build dir is different to the source dir

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(SEND_ERROR "${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run cmake there.")
	message(FATAL_ERROR "Remove the file CMakeCache.txt in ${CMAKE_SOURCE_DIR} first.")
endif()
