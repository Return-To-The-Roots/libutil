# Builds the libraries for Boost-Nowide
# Automatically detects if build as standalone or as "part of boost" (with boost namespace)
#
# Options:
# NOWIDE_USE_BOOST_FILESYSTEM
# NOWIDE_BUILD_SHARED           Build target nowide
# NOWIDE_BUILD_STATIC           Build target nowide-static
#
# Exported targets:
# nowide::interface
# nowide::static
# nowide::shared
#
# Default is to build the static library on windows and the dynamic library on other systems (Similar to boost)
# You can also build both at the same time.
#
# Non-standalone version:
# Setting NOWIDE_USE_BOOST_FILESYSTEM is not required to accept boost::filesystem::path but it adds the Boost.Filesystem dependency and defines
# -DBOOST_NOWIDE_USE_FILESYSTEM for targets linking agains nowide. You can also define this manually.
# Linking against the shared library (nowide) requires -DBOOST_NOWIDE_DYN_LINK on windows. It is also adviced to define -D BOOST_NOWIDE_NO_LIB
# to disable autolinking as the generated library name is non-default
#
# Standalone version:
# NOWIDE_USE_BOOST_FILESYSTEM is ignored.
# Linking against the shared library (nowide) requires -DNOWIDE_EXPORT on windows.

include(GNUInstallDirs)
include(CMakeDependentOption)

if(WIN32)
    option(NOWIDE_BUILD_SHARED "Build shared library" OFF)
    option(NOWIDE_BUILD_STATIC "Build static library" ON)
else()
    option(NOWIDE_BUILD_SHARED "Build shared library" ON)
    option(NOWIDE_BUILD_STATIC "Build static library" OFF)
endif()
option(NOWIDE_SYSTEM_INCLUDE "Treat Boost.Nowide includes as system includes" ON)

# Get path to nowide
get_filename_component(NOWIDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

add_library(nowide-interface INTERFACE)
add_library(nowide::interface ALIAS nowide-interface)
if(NOWIDE_SYSTEM_INCLUDE)
    target_include_directories(nowide-interface SYSTEM INTERFACE ${NOWIDE_DIR}/include)
else()
    target_include_directories(nowide-interface INTERFACE ${NOWIDE_DIR}/include)
endif()

# Find out if we are beeing build as standalone or boost version
if(EXISTS ${NOWIDE_DIR}/include/nowide)
	set(NOWIDE_STANDALONE ON CACHE INTERNAL "Build without boost")
	file(GLOB NOWIDE_HEADERS ${NOWIDE_DIR}/include/nowide/*.hpp)
else()
	# Default boost libs are static on self-build msvc versions and dynamic in the linux package repos
	if(MSVC AND "${Boost_USE_STATIC_LIBS}" STREQUAL "")
		set(Boost_USE_STATIC_LIBS ON CACHE BOOL "")
	endif()

	if(NOWIDE_USE_BOOST_FILESYSTEM)
		find_package(Boost REQUIRED COMPONENTS filesystem)
        target_link_libraries(nowide-interface INTERFACE Boost::filesystem)
        target_compile_definitions(nowide-interface INTERFACE BOOST_NOWIDE_USE_FILESYSTEM)
    else()
        find_package(Boost REQUIRED)
        target_link_libraries(nowide-interface INTERFACE Boost::boost)
	endif()
	set(NOWIDE_STANDALONE OFF CACHE INTERNAL "Build with boost")
    file(GLOB NOWIDE_HEADERS ${NOWIDE_DIR}/include/boost/nowide/*.hpp)
endif()

CMAKE_DEPENDENT_OPTION(NOWIDE_USE_BOOST_FILESYSTEM "Build with support for boost filesystem" OFF
                       "NOT NOWIDE_STANDALONE" OFF)

if(NOWIDE_BUILD_SHARED)
	add_library(nowide-shared SHARED iostream.cpp ${NOWIDE_HEADERS})
    add_library(nowide::shared ALIAS nowide-shared)
	set_target_properties(nowide-shared PROPERTIES OUTPUT_NAME "nowide" VERSION 0.0.0 SOVERSION 0)
    target_link_libraries(nowide-shared PUBLIC nowide::interface)

	if(NOWIDE_STANDALONE)
		target_compile_definitions(nowide-shared PUBLIC NOWIDE_EXPORT)
	else()
		target_compile_definitions(nowide-shared PUBLIC BOOST_NOWIDE_DYN_LINK BOOST_NOWIDE_NO_LIB)
	endif()

	install(TARGETS nowide-shared
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(NOWIDE_BUILD_STATIC)
	add_library(nowide-static STATIC iostream.cpp ${NOWIDE_HEADERS})
    add_library(nowide::static ALIAS nowide-static)
	# Rename to libnowide and enable linking into shared libs
	set_target_properties(nowide-static PROPERTIES OUTPUT_NAME "nowide" PREFIX "lib" POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(nowide-static PUBLIC nowide::interface)
	install(TARGETS nowide-static
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

install(DIRECTORY ${NOWIDE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})