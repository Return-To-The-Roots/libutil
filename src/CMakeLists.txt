#################################################################################
### $Id: CMakeLists.txt 7521 2011-09-08 20:45:55Z FloSoft $
#################################################################################

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}
	"${CMAKE_SOURCE_DIR}/libendian/src/"
	${BOOST_INCLUDE_DIR}
	"/usr/local/include")
LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/libendian/src")

#################################################################################

FILE(GLOB SRC_FILES *.cpp *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES})

IF (MSVC)
	SET( UPNC_LIB "")
ELSE()
	SET( UPNC_LIB "miniupnpc")
ENDIF()

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util ${UPNC_LIB} iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util ${UPNC_LIB} ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
