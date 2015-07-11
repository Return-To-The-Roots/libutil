#################################################################################
### $Id: CMakeLists.txt 7521 2011-09-08 20:45:55Z FloSoft $
#################################################################################

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/libendian/src/")
LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/libendian/src")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp MessageInterface.cpp Message.cpp Serializer.cpp files.cpp UPnP.cpp BinaryFile.h BinaryFile.cpp)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
