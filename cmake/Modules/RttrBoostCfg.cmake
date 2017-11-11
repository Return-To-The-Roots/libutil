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
else(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost dynamic libraries")
endif(Boost_USE_STATIC_LIBS)

# For some reason boost needs sometimes 1.xy and sometimes 1.xy.0 to detect folder structures like boost_1_55_0
SET(TMP_VERSIONS "1.65" "1.64" "1.63" "1.62" "1.61" "1.60" "1.59" "1.58" "1.57" "1.56" "1.55")
foreach(I_VERSION ${TMP_VERSIONS})
	list(APPEND Boost_ADDITIONAL_VERSIONS "${I_VERSION}.1" "${I_VERSION}.0" "${I_VERSION}")
endforeach()
UNSET(TMP_VERSIONS)