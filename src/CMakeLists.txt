set(_BoostComponents system filesystem random)
if(WIN32)
	# Locale only on windows
	set(_BoostComponents ${_BoostComponents} locale)
endif()
FIND_PACKAGE(Boost 1.55 REQUIRED COMPONENTS ${_BoostComponents})
IF (NOT MSVC)
	FIND_PACKAGE(Miniupnpc REQUIRED)
    include(GatherDll)
    gather_dll(MINIUPNPC)
ENDIF()

#################################################################################

FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB HEADER_FILES ${includePath}/libutil/*.* *.h)
ADD_LIBRARY(s25util STATIC ${SRC_FILES} ${HEADER_FILES})

# Required for linking into shared libs
set_property(TARGET s25util PROPERTY POSITION_INDEPENDENT_CODE ON)

get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)
target_include_directories(s25util
    INTERFACE ${includePath}
    PRIVATE ${includePath}/libutil
    SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(s25util PUBLIC endian nowide-static utf8 ${Boost_LIBRARIES})
target_compile_definitions(s25util PUBLIC "$<$<NOT:$<COMPILE_FEATURES:cxx_override>>:override=>")

if(MSVC)
    target_compile_definitions(s25util PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
	target_link_libraries(s25util PUBLIC ${MINIUPNPC_LIBRARY})
    target_include_directories(s25util SYSTEM PRIVATE ${MINIUPNPC_INCLUDE_DIR})
ENDIF()

IF (WIN32)
    target_compile_definitions(s25util PRIVATE -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WINXP)
    # Exclude some windows defines
    target_compile_definitions(s25util PRIVATE -DNOMINMAX -DNODRAWTEXT -DNOSOUND -DNOTEXTMETRIC -DNOCOMM -DNOMCX)
    # Disable automatic boost linking
    target_compile_definitions(s25util PUBLIC -DBOOST_ALL_NO_LIB)
	target_link_libraries(s25util PUBLIC iphlpapi PUBLIC ws2_32)
ENDIF ()

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES} ${HEADER_FILES})
endif()
