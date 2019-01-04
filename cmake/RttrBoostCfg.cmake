#
# Init RTTR_USE_STATIC_BOOST option
# Show message about boost config
# Add additional boost versions to Boost_ADDITIONAL_VERSIONS

# Default boost libs are static on self-build windows versions and dynamic in the linux package repos
IF(WIN32 OR APPLE)
	SET(RTTR_USE_STATIC_BOOST_DEFAULT ON)
ELSE()
	SET(RTTR_USE_STATIC_BOOST_DEFAULT OFF)
ENDIF()

OPTION(RTTR_USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" ${RTTR_USE_STATIC_BOOST_DEFAULT})
IF((NOT "${Boost_USE_STATIC_LIBS}" AND ${RTTR_USE_STATIC_BOOST}) OR
   ("${Boost_USE_STATIC_LIBS}" AND NOT ${RTTR_USE_STATIC_BOOST}))
    set(Boost_USE_STATIC_LIBS ${RTTR_USE_STATIC_BOOST})
    unset(Boost_INCLUDE_DIR CACHE)
    unset(Boost_LIBRARY_DIRS CACHE)
ENDIF()
if(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost static libraries")
else()
   message(STATUS "Linking against boost dynamic or static libraries")
endif()
