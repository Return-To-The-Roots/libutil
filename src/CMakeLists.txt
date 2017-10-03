FIND_PACKAGE(Boost 1.55 COMPONENTS system filesystem REQUIRED)
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
ENDIF()

get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)
INCLUDE_DIRECTORIES(${includePath}/libutil ${UTF8_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS} ${MINIUPNPC_INCLUDE_DIR})

#################################################################################

FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB HEADER_FILES ${includePath}/libutil/*.* *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES} ${HEADER_FILES})

# Required for linking into shared libs
set_property(TARGET s25util PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(s25util INTERFACE ${includePath})
TARGET_LINK_LIBRARIES(s25util endian nowide-static ${Boost_LIBRARIES})

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES(s25util ${MINIUPNPC_LIBRARY})
ENDIF()

IF (WIN32)
	TARGET_LINK_LIBRARIES(s25util iphlpapi ws2_32)
ENDIF ()

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES} ${HEADER_FILES})
endif()
