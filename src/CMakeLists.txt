FIND_PACKAGE(Boost 1.55 COMPONENTS system filesystem REQUIRED)
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
ENDIF()


INCLUDE_DIRECTORIES(${CMAKE_PROJECT_DIR} ${Boost_INCLUDE_DIR} ${MINIUPNPC_INCLUDE_DIR})

INCLUDE(CheckCXXCompilerFlag)
IF (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	# Required for linking into shared libs (CMake 2.8.9 up could use the property below)
	CHECK_CXX_COMPILER_FLAG(-fPIC FLAG_FPIC_SUPPORTED)
	IF(FLAG_FPIC_SUPPORTED)
		ADD_DEFINITIONS(-fPIC)
	ENDIF()
ENDIF()

#################################################################################

FILE(GLOB SRC_FILES *.cpp *.h *.hpp *.tpp)
ADD_LIBRARY(s25util STATIC ${SRC_FILES})

IF (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	# Required for linking into shared libs
	set_property(TARGET s25util PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF()

TARGET_LINK_LIBRARIES(s25util ${Boost_LIBRARIES})

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES(s25util ${MINIUPNPC_LIBRARY})
ENDIF()

IF ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	TARGET_LINK_LIBRARIES(s25util iphlpapi ws2_32)
ENDIF ()

#################################################################################
