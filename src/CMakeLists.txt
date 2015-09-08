FIND_PACKAGE(Boost 1.55 COMPONENTS system filesystem REQUIRED)
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
ENDIF()


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}
	"${CMAKE_SOURCE_DIR}/libendian/src/"
	${Boost_INCLUDE_DIR}
	${MINIUPNPC_INCLUDE_DIR})
LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/libendian/src")

#################################################################################

FILE(GLOB SRC_FILES *.cpp *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES})

TARGET_LINK_LIBRARIES(s25util ${Boost_LIBRARIES})

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES(s25util miniupnpc)
ENDIF()

IF ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	TARGET_LINK_LIBRARIES(s25util iphlpapi ws2_32)
ENDIF ()

#################################################################################
