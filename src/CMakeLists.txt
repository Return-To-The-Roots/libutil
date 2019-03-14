set(_BoostComponents filesystem)
if(WIN32)
	# Locale only on windows
	set(_BoostComponents ${_BoostComponents} locale)
endif()
FIND_PACKAGE(Boost 1.64 REQUIRED COMPONENTS ${_BoostComponents})
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
    include(GatherDll)
    gather_dll(MINIUPNPC)
ENDIF()

#################################################################################

get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)

FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB HEADER_FILES ${includePath}/libutil/*.* *.h)
if(WIN32)
    list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/System_Other.cpp)
else()
    list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/System_Win32.cpp)
endif()

ADD_LIBRARY(s25util STATIC ${SRC_FILES} ${HEADER_FILES})

target_include_directories(s25util
    INTERFACE ${includePath}
    PRIVATE ${includePath}/libutil)
target_link_libraries(s25util PUBLIC endian::static nowide::static Boost::filesystem PRIVATE utf8)
target_compile_features(s25util PUBLIC cxx_std_14)
# PIC Required for linking into shared libs
set_target_properties(s25util PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    target_compile_definitions(s25util PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
	target_link_libraries(s25util PRIVATE ${MINIUPNPC_LIBRARY})
    target_include_directories(s25util SYSTEM PRIVATE ${MINIUPNPC_INCLUDE_DIR})
ENDIF()

IF (WIN32)
    target_compile_definitions(s25util PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600) # Win Vista
    # Exclude some windows defines
    target_compile_definitions(s25util PRIVATE NOMINMAX NODRAWTEXT NOSOUND NOTEXTMETRIC NOCOMM NOMCX)
	target_link_libraries(s25util PUBLIC Boost::locale Boost::disable_autolinking iphlpapi ws2_32)
ENDIF ()

include(EnableWarnings)
enable_warnings(s25util)
