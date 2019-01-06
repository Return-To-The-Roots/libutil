# Builds the libraries for Boost-Nowide
# Automatically detects if build as standalone or as "part of boost" (with boost namespace)
#
# Options:
# NOWIDE_USE_BOOST_FILESYSTEM
# NOWIDE_BUILD_SHARED           Build target nowide::shared
# NOWIDE_BUILD_STATIC           Build target nowide::static
#
# Exported targets:
# nowide::static
# nowide::shared
# Both are aliases for nowide::interface on non-windows systems since implementations are only required on windows.
# nowide::nowide is defined as an alias if only one of them is build
#
# Default is to build the static library on windows (similar to boost)
# You can also build both at the same time.
#
# Non-standalone version:
# Setting NOWIDE_USE_BOOST_FILESYSTEM is not required to accept boost::filesystem::path but it adds the Boost.Filesystem dependency and defines
# -DBOOST_NOWIDE_USE_FILESYSTEM for targets linking agains nowide. You can also define this manually.
#
# Standalone version:
# NOWIDE_USE_BOOST_FILESYSTEM is ignored.

project(Boost.nowide)

include(GNUInstallDirs)
include(CMakeDependentOption)

add_library(nowide-interface INTERFACE)
add_library(nowide::interface ALIAS nowide-interface)
target_include_directories(nowide-interface INTERFACE include)

# Find out if we are beeing build as standalone or boost version
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/nowide)
	set(NOWIDE_STANDALONE ON CACHE INTERNAL "Build without boost")
	file(GLOB NOWIDE_HEADERS include/nowide/*.hpp)
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
    file(GLOB NOWIDE_HEADERS include/boost/nowide/*.hpp)
endif()

cmake_dependent_option(NOWIDE_USE_BOOST_FILESYSTEM "Build with support for boost filesystem" OFF
                       "NOT NOWIDE_STANDALONE" OFF)

if(WIN32)
	set(NOWIDE_SRCS src/cstdio.cpp src/iostream.cpp src/system.cpp  ${NOWIDE_HEADERS})

	if(NOWIDE_BUILD_SHARED)
		add_library(nowide-shared SHARED ${NOWIDE_SRCS})
		add_library(nowide::shared ALIAS nowide-shared)
		set_target_properties(nowide-shared PROPERTIES OUTPUT_NAME "nowide" VERSION 0.0.0 SOVERSION 0)
		target_link_libraries(nowide-shared PUBLIC nowide::interface)
		if(MSVC)
			target_compile_options(nowide-shared PRIVATE /wd4996)
		endif()
		# Define import/export flags
		if(NOWIDE_STANDALONE)
			target_compile_definitions(nowide-shared PUBLIC NOWIDE_EXPORT)
		else()
			# BOOST_NOWIDE_NO_LIB required as created lib names are non-default
			target_compile_definitions(nowide-shared PUBLIC BOOST_NOWIDE_DYN_LINK BOOST_NOWIDE_NO_LIB)
		endif()

		install(TARGETS nowide-shared
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
	endif()

	if(NOWIDE_BUILD_STATIC)
		add_library(nowide-static STATIC ${NOWIDE_SRCS})
		add_library(nowide::static ALIAS nowide-static)
		# Rename to libnowide and enable linking into shared libs
		set_target_properties(nowide-static PROPERTIES OUTPUT_NAME "nowide" PREFIX "lib" POSITION_INDEPENDENT_CODE ON)
		target_link_libraries(nowide-static PUBLIC nowide::interface)
		if(MSVC)
			target_compile_options(nowide-static PRIVATE /wd4996)
		endif()
		install(TARGETS nowide-static
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
	endif()
	if(NOWIDE_BUILD_SHARED AND NOT NOWIDE_BUILD_STATIC)
		add_library(nowide::nowide ALIAS nowide-shared)
	elseif(NOT NOWIDE_BUILD_SHARED AND NOWIDE_BUILD_STATIC)
		add_library(nowide::nowide ALIAS nowide-static)
	endif()
else()
	if(NOWIDE_BUILD_SHARED)
		add_library(nowide::shared ALIAS nowide-interface)
	endif()
	if(NOWIDE_BUILD_STATIC)
		add_library(nowide::static ALIAS nowide-interface)
	endif()
	if(NOWIDE_BUILD_SHARED AND NOT NOWIDE_BUILD_STATIC)
		add_library(nowide::nowide ALIAS nowide-interface)
	elseif(NOT NOWIDE_BUILD_SHARED AND NOWIDE_BUILD_STATIC)
		add_library(nowide::nowide ALIAS nowide-interface)
	endif()
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
