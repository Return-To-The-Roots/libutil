# Find module which defines the target LUA::LUA
# For MSVC it also sets LUA_DLL

function(_lua_extract_header_version)
	set(LUA_VERSION_STRING "" PARENT_SCOPE)
	if(LUA_INCLUDE_DIR)
		set(LUA_VERSION_REGEX "^#define LUA_VERSION_(MAJOR|MINOR|RELEASE) +\"([0-9]+)\"$")
		file(STRINGS "${LUA_INCLUDE_DIR}/lua.h" LUA_VERSION_STRINGS REGEX ${LUA_VERSION_REGEX})
		set(LUA_VERSION_OK TRUE)
		foreach(subVer MAJOR MINOR RELEASE)
			if(LUA_VERSION_STRINGS MATCHES "#define LUA_VERSION_${subVer} +\"([0-9]+)\"")
				set(LUA_VERSION_${subVer} ${CMAKE_MATCH_1})
			else()
				unset(LUA_VERSION_${subVer})
				set(LUA_VERSION_OK FALSE)
			endif()
			set(LUA_VERSION_${subVer} ${LUA_VERSION_${subVer}} PARENT_SCOPE)
		endforeach()
		if(LUA_VERSION_OK)
			set(LUA_VERSION_PATCH ${LUA_VERSION_RELEASE} PARENT_SCOPE)
			set(LUA_VERSION_STRING "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_RELEASE}" PARENT_SCOPE)
		endif()
	endif()
endfunction()

macro(_lua_set_hints suffix)
	unset(_LUA_HINTS)
	if(LUA_DIR)
		list(APPEND _LUA_HINTS ${LUA_DIR} ${LUA_DIR}/${suffix})
	endif()
	if(ENV{LUA_DIR})
		list(APPEND _LUA_HINTS $ENV{LUA_DIR} $ENV{LUA_DIR}/${suffix})
	endif()
endmacro()

# Libraries and folders may contain major and minor version. List them here
set(_LUA_VERSIONS 5.3 5.2)
if(LUA_FIND_VERSION_EXACT)
	if(LUA_FIND_VERSION_COUNT GREATER 1)
		set(_LUA_VERSIONS ${LUA_FIND_VERSION_MAJOR}.${LUA_FIND_VERSION_MINOR})
	endif ()
elseif(LUA_FIND_VERSION)
	list(INSERT 0 LUA_VERSIONS ${LUA_FIND_VERSION})
endif()

# Add possible include suffixes
unset(_LUA_SUFFIXES)
foreach(version ${LUA_VERSIONS})
	if(version MATCHES "^([0-9]+)\\.([0-9]+)$")
		list(APPEND _LUA_SUFFIXES
		     lua${CMAKE_MATCH_1}${CMAKE_MATCH_2}
		     lua${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
		     lua-${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
		)
	endif()
endforeach()
list(APPEND _LUA_SUFFIXES lua include)

_lua_set_hints(include)
find_path(LUA_INCLUDE_DIR "lua.h"
	HINTS ${_LUA_HINTS}
	PATH_SUFFIXES ${_LUA_SUFFIXES}
)

_lua_extract_header_version()

if(LUA_VERSION_STRING)
	set(_LUA_VERSIONS ${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR})
endif()

set(_LUA_POSSIBLE_LIB_NAMES )
# For non-MSVC prefer static libraries
if(NOT MSVC)
	foreach(version ${_LUA_VERSIONS})
		string(REPLACE "." "" versionNoDot ${version})
		list(APPEND _LUA_POSSIBLE_LIB_NAMES liblua${versionNoDot}.a liblua${version}.a liblua-${version}.a liblua.${version}.a )
	endforeach()
endif()
# Prefer explicitely versioned libraries
foreach(version ${_LUA_VERSIONS})
	string(REPLACE "." "" versionNoDot ${version})
	list(APPEND _LUA_POSSIBLE_LIB_NAMES lua${versionNoDot} lua${version} lua-${version} lua.${version})
endforeach()
# Fallback to unversioned lua
if(NOT MSVC)
	list(APPEND _LUA_POSSIBLE_LIB_NAMES liblua.a)
endif()
list(APPEND _LUA_POSSIBLE_LIB_NAMES lua)

# Try parent folder of include dir
if(LUA_INCLUDE_DIR)
	get_filename_component(_LUA_HINTS ${LUA_INCLUDE_DIR} DIRECTORY)
	find_library(LUA_LIBRARY
		NAMES ${_LUA_POSSIBLE_LIB_NAMES}
		PATHS ${_LUA_HINTS} ${_LUA_HINTS}/lib
		NO_DEFAULT_PATH
	)
endif()

_lua_set_hints(lib)
find_library(LUA_LIBRARY
	NAMES ${_LUA_POSSIBLE_LIB_NAMES}
	HINTS ${_LUA_HINTS}
)

if(NOT LUA_INCLUDE_DIR AND LUA_LIBRARY)
	# Find include with help of LUA_LIBRARY
	get_filename_component(_LUA_PARENT ${LUA_LIBRARY} DIRECTORY)
	get_filename_component(_LUA_PARENT ${_LUA_PARENT} DIRECTORY)
	set(_LUA_HINTS ${_LUA_PARENT})
	if(_LUA_PARENT MATCHES "^(.*)/lib$")
		list(APPEND _LUA_HINTS ${CMAKE_MATCH_1} ${CMAKE_MATCH_1}/include)
	endif()
	list(APPEND _LUA_HINTS ${_LUA_PARENT}/include)
	find_path(LUA_INCLUDE_DIR "lua.h"
		PATHS ${_LUA_HINTS}
		PATH_SUFFIXES ${_LUA_SUFFIXES}
		NO_DEFAULT_PATH
	)

	_lua_extract_header_version()	
endif()


# Extract version
if(LUA_LIBRARY AND LUA_VERSION_STRING AND ${LUA_LIBRARY} MATCHES "^.*lua(-|\\.)?([0-9])\\.?([0-9])\\.[a-zA-Z]+$")
	set(LUA_LIB_VERSION "${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
	if(NOT LUA_LIB_VERSION VERSION_EQUAL "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}")
		message(FATAL_ERROR "Incompatible version of ${LUA_LIBRARY} (${LUA_LIB_VERSION}) to that found in ${LUA_INCLUDE_DIR}/lua.h (${LUA_VERSION_STRING})")
	endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LUA
	REQUIRED_VARS LUA_INCLUDE_DIR LUA_LIBRARY
	VERSION_VAR LUA_VERSION_STRING
)

MARK_AS_ADVANCED(
  LUA_INCLUDE_DIR
  LUA_LIBRARY
)

if(LUA_FOUND)
	add_library(lua::lua UNKNOWN IMPORTED)
	set_target_properties(lua::lua PROPERTIES
		INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR}
		IMPORTED_LOCATION ${LUA_LIBRARY}
	)
	# Need math library and dl library on UNIX
	if(UNIX AND NOT APPLE AND NOT BEOS)
		set_property(TARGET lua::lua PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES m ${CMAKE_DL_LIBS})
	endif()
endif()

