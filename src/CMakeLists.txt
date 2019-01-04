set(_BoostComponents filesystem random)
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
ADD_LIBRARY(s25util STATIC ${SRC_FILES} ${HEADER_FILES})

# Required for linking into shared libs
set_property(TARGET s25util PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(s25util
    INTERFACE ${includePath}
    PRIVATE ${includePath}/libutil)
target_link_libraries(s25util PUBLIC endian::static nowide::static Boost::filesystem PRIVATE utf8 Boost::random)
target_compile_definitions(s25util PUBLIC "$<$<NOT:$<COMPILE_FEATURES:cxx_override>>:override=>")

if(MSVC)
    target_compile_definitions(s25util PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
	target_link_libraries(s25util PRIVATE ${MINIUPNPC_LIBRARY})
    target_include_directories(s25util SYSTEM PRIVATE ${MINIUPNPC_INCLUDE_DIR})
ENDIF()

IF (WIN32)
    target_compile_definitions(s25util PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=_WIN32_WINNT_WINXP)
    # Exclude some windows defines
    target_compile_definitions(s25util PRIVATE NOMINMAX NODRAWTEXT NOSOUND NOTEXTMETRIC NOCOMM NOMCX)
	target_link_libraries(s25util PUBLIC Boost::locale Boost::disable_autolinking iphlpapi ws2_32)
ENDIF ()

include(EnableWarnings)
enable_warnings(s25util)

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES} ${HEADER_FILES})
endif()
