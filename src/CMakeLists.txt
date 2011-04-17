#################################################################################
### $Id: CMakeLists.txt 7178 2011-04-17 15:53:20Z FloSoft $
#################################################################################

INCLUDE_DIRECTORIES("../../" "../../libendian/src/")
LINK_DIRECTORIES("../../libendian/src")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp SerializableArray.cpp Messages.cpp MessageInterface.cpp Message.cpp Serializer.cpp files.cpp UPnP.cpp BinaryFile.h BinaryFile.cpp)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util miniupnpc ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
