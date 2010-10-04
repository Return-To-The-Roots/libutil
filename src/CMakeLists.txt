#################################################################################
### $Id: CMakeLists.txt 6474 2010-06-03 12:22:29Z FloSoft $
#################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################

INCLUDE_DIRECTORIES("../../")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp SerializableArray.cpp Messages.cpp MessageInterface.cpp Message.cpp Serializer.cpp files.cpp UPnP.cpp)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
