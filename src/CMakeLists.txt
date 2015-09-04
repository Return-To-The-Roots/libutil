
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
ENDIF()


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}
	"${CMAKE_SOURCE_DIR}/libendian/src/"
	${BOOST_INCLUDE_DIR}
	${MINIUPNPC_INCLUDE_DIR})
LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/libendian/src")

#################################################################################

FILE(GLOB SRC_FILES *.cpp *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES})

IF (MSVC)
	SET( UPNC_LIB "")
ELSE()
	SET( UPNC_LIB "miniupnpc")
ENDIF()

IF ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	TARGET_LINK_LIBRARIES(s25util ${UPNC_LIB} iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	TARGET_LINK_LIBRARIES(s25util ${UPNC_LIB} ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )

#################################################################################
