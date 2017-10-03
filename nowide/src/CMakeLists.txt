# Builds the libraries for Boost-Nowide
# Automatically detects if build as standalone or as "part of boost" (with boost namespace)
#
# Options:
# NOWIDE_USE_BOOST_FILESYSTEM
# NOWIDE_BUILD_SHARED           Build target nowide
# NOWIDE_BUILD_STATIC           Build target nowide-static
#
# Exported cache variables:
# NOWIDE_INCLUDE_DIRS
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

# Get path to nowide
get_filename_component(NOWIDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(NOWIDE_INCLUDE_DIR ${NOWIDE_DIR}/include)
set(NOWIDE_INCLUDE_DIRS ${NOWIDE_INCLUDE_DIR} CACHE PATH "Path to nowide includes")
# Find out if we are beeing build as standalone or boost version
if(EXISTS ${NOWIDE_INCLUDE_DIR}/nowide)
	set(NOWIDE_STANDALONE ON CACHE INTERNAL "Build without boost")
	set(NOWIDE_HEADER_DIR ${NOWIDE_INCLUDE_DIR}/nowide)
else()
	# Default boost libs are static on self-build msvc versions and dynamic in the linux package repos
	if(MSVC AND "${Boost_USE_STATIC_LIBS}" STREQUAL "")
		set(Boost_USE_STATIC_LIBS ON)
	endif()

	if(NOWIDE_USE_BOOST_FILESYSTEM)
		find_package(Boost 1.55 REQUIRED COMPONENTS Filesystem System)
    else()
        find_package(Boost 1.55 REQUIRED)
	endif()
	set(NOWIDE_STANDALONE OFF CACHE INTERNAL "Build with boost")
	set(NOWIDE_HEADER_DIR ${NOWIDE_INCLUDE_DIR}/boost/nowide)
endif()

CMAKE_DEPENDENT_OPTION(NOWIDE_USE_BOOST_FILESYSTEM "Build with support for boost filesystem" OFF
                       "NOT NOWIDE_STANDALONE" OFF)

# Using glob here is ok as it is only for headers
file(GLOB NOWIDE_HEADERS ${NOWIDE_HEADER_DIR}/*.hpp)

if(NOWIDE_BUILD_SHARED)
	add_library(nowide SHARED iostream.cpp ${NOWIDE_HEADERS})
	set_target_properties(nowide PROPERTIES VERSION 0.0.0 SOVERSION 0)

	target_include_directories(nowide PUBLIC ${NOWIDE_DIR}/include)
	if(NOWIDE_STANDALONE)
		target_compile_definitions(nowide PUBLIC NOWIDE_EXPORT PRIVATE NOWIDE_SOURCE)
	else()
		target_compile_definitions(nowide PUBLIC BOOST_NOWIDE_DYN_LINK BOOST_NOWIDE_NO_LIB PRIVATE BOOST_NOWIDE_SOURCE)
		target_include_directories(nowide SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
		#target_link_libraries(nowide INTERFACE ${Boost_LOCALE_LIBRARY})
		if(NOWIDE_USE_BOOST_FILESYSTEM)
			target_compile_definitions(nowide PUBLIC BOOST_NOWIDE_USE_FILESYSTEM)
			target_link_libraries(nowide INTERFACE ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
		endif()
	endif()

	install(TARGETS nowide
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(NOWIDE_BUILD_STATIC)
	add_library(nowide-static STATIC iostream.cpp ${NOWIDE_HEADERS})
	# Rename to libnowide and enable linking into shared libs
	set_target_properties(nowide-static PROPERTIES OUTPUT_NAME "nowide" PREFIX "lib" POSITION_INDEPENDENT_CODE ON)

	target_include_directories(nowide-static PUBLIC ${NOWIDE_DIR}/include)
	if(NOT NOWIDE_STANDALONE)
		target_include_directories(nowide-static SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
		target_compile_definitions(nowide-static PUBLIC BOOST_NOWIDE_NO_LIB)
		#target_link_libraries(nowide-static ${Boost_LOCALE_LIBRARY})
		if(NOWIDE_USE_BOOST_FILESYSTEM)
            target_compile_definitions(nowide-static PUBLIC BOOST_NOWIDE_USE_FILESYSTEM)
			target_link_libraries(nowide-static ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
		endif()
	endif()
	install(TARGETS nowide-static
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

install(DIRECTORY ${NOWIDE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})