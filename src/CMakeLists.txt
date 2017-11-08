FIND_PACKAGE(Boost 1.55 REQUIRED COMPONENTS system filesystem locale)
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
    include(GatherDll)
    gather_dll(MINIUPNPC)
ENDIF()

get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)
INCLUDE_DIRECTORIES(${includePath}/libutil ${UTF8_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS} ${MINIUPNPC_INCLUDE_DIR})

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WINXP)
    # Exclude some windows defines
    add_definitions(-DNOMINMAX -DNODRAWTEXT -DNOSOUND -DNOTEXTMETRIC -DNOCOMM -DNOMCX)
endif()

#################################################################################

FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB HEADER_FILES ${includePath}/libutil/*.* *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES} ${HEADER_FILES})

# Required for linking into shared libs
set_property(TARGET s25util PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(s25util INTERFACE ${includePath})
TARGET_LINK_LIBRARIES(s25util PUBLIC endian PUBLIC nowide-static PUBLIC ${Boost_LIBRARIES})

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES(s25util PUBLIC ${MINIUPNPC_LIBRARY})
ENDIF()

IF (WIN32)
	TARGET_LINK_LIBRARIES(s25util PUBLIC iphlpapi PUBLIC ws2_32)
ENDIF ()

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES} ${HEADER_FILES})
endif()
