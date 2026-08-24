# Copyright (C) 2005 - 2026 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

#
# Init RTTR_USE_STATIC_BOOST option
# Show message about boost config

# Default boost libs are static on self-build windows versions and dynamic in the linux package repos
if(WIN32 AND NOT MINGW)
	set(RTTR_USE_STATIC_BOOST_DEFAULT ON)
else()
	set(RTTR_USE_STATIC_BOOST_DEFAULT OFF)
endif()

option(RTTR_USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" ${RTTR_USE_STATIC_BOOST_DEFAULT})

set(Boost_USE_STATIC_LIBS ${RTTR_USE_STATIC_BOOST})

if(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against static boost libraries")
else()
   message(STATUS "Linking against dynamic boost libraries")
endif()
