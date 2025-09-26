# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

#
# Init RTTR_USE_STATIC_BOOST option
# Show message about boost config
# Add additional boost versions to Boost_ADDITIONAL_VERSIONS

# Default boost libs are static on self-build windows versions and dynamic in the linux package repos
if(WIN32 AND NOT MINGW)
	set(RTTR_USE_STATIC_BOOST_DEFAULT ON)
else()
	set(RTTR_USE_STATIC_BOOST_DEFAULT OFF)
endif()

option(RTTR_USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" ${RTTR_USE_STATIC_BOOST_DEFAULT})
if((NOT Boost_USE_STATIC_LIBS AND RTTR_USE_STATIC_BOOST) OR
   (Boost_USE_STATIC_LIBS AND NOT RTTR_USE_STATIC_BOOST))
    unset(Boost_INCLUDE_DIR CACHE)
    unset(Boost_LIBRARY_DIRS CACHE)
endif()

# Need to be explicitely set
set(Boost_USE_STATIC_LIBS ${RTTR_USE_STATIC_BOOST})

if(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost static libraries")
else()
   message(STATUS "Linking against boost dynamic or static libraries")
endif()
