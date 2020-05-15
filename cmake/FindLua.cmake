# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindLua
# -------
#
#
#
# Locate Lua library This module defines
#
# ::
#
#   LUA_FOUND          - if false, do not try to link to Lua
#   LUA_LIBRARIES      - both lua and lualib
#   LUA_INCLUDE_DIR    - where to find lua.h
#   LUA_VERSION_STRING - the version of Lua found
#   LUA_VERSION_MAJOR  - the major version of Lua
#   LUA_VERSION_MINOR  - the minor version of Lua
#   LUA_VERSION_PATCH  - the patch version of Lua
#
#
#
# Note that the expected include convention is
#
# ::
#
#   #include "lua.h"
#
# and not
#
# ::
#
#   #include <lua/lua.h>
#
# This is because, the lua location is not standardized and may exist in
# locations other than lua/

unset(_lua_include_subdirs)
unset(_lua_library_names)
unset(_lua_considered_versions)
set(_lua_additional_paths
      ~/Library/Frameworks
      /Library/Frameworks
      /sw # Fink
      /opt/local # DarwinPorts
      /opt/csw # Blastwave
      /opt)

function(_lua_debug_msg msg)
	if(LUA_Debug)
		message(STATUS "${msg}")
	endif()
endfunction()

# this is a function only to have all the variables inside go away automatically
function(_lua_get_versions)
    set(LUA_VERSIONS5 5.3 5.2 5.1 5.0)

    if(Lua_FIND_VERSION_EXACT)
        if(Lua_FIND_VERSION_COUNT GREATER 1)
            set(_lua_considered_versions ${Lua_FIND_VERSION_MAJOR}.${Lua_FIND_VERSION_MINOR})
        endif()
    elseif(Lua_FIND_VERSION)
        # once there is a different major version supported this should become a loop
        if(NOT Lua_FIND_VERSION_MAJOR GREATER 5)
            if(Lua_FIND_VERSION_COUNT EQUAL 1)
                set(_lua_considered_versions ${LUA_VERSIONS5})
            else()
                foreach(subver IN LISTS LUA_VERSIONS5)
                    if(NOT subver VERSION_LESS ${Lua_FIND_VERSION})
                        list(APPEND _lua_considered_versions ${subver})
                    endif()
                endforeach()
                # New version -> Search for it (heuristic only! Defines in include might have changed)
                if(NOT _lua_considered_versions)
                    set(_lua_considered_versions ${Lua_FIND_VERSION_MAJOR}.${Lua_FIND_VERSION_MINOR})
                endif()
            endif()
        endif()
    else()
        # once there is a different major version supported this should become a loop
        set(_lua_considered_versions ${LUA_VERSIONS5})
    endif()

    _lua_debug_msg("Considering following Lua versions: ${_lua_considered_versions}")

    set(_lua_considered_versions "${_lua_considered_versions}" PARENT_SCOPE)
endfunction()

function(_lua_get_header_dir_candidates)
   set(versioned_dirs lua)

    foreach(ver IN LISTS _lua_considered_versions)
        string(REGEX MATCH "^([0-9]+)\\.([0-9]+)$" _ver "${ver}")
        list(APPEND versioned_dirs
             lua${CMAKE_MATCH_1}${CMAKE_MATCH_2}
             lua${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             lua-${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
        )
    endforeach()

    set(_lua_include_subdirs include)
    # Prepend "include/" to each path directly after the path
    foreach(dir IN LISTS versioned_dirs)
        list(APPEND _lua_include_subdirs "${dir}" "include/${dir}")
    endforeach()

    set(_lua_include_subdirs "${_lua_include_subdirs}" PARENT_SCOPE)
endfunction()

function(_lua_get_header_version)
    unset(LUA_VERSION_STRING PARENT_SCOPE)
    set(_hdr_file "${LUA_INCLUDE_DIR}/lua.h")

    if(NOT EXISTS "${_hdr_file}")
        return()
    endif()

    # At least 5.[012] have different ways to express the version
    # so all of them need to be tested. Lua 5.2 defines LUA_VERSION
    # and LUA_RELEASE as joined by the C preprocessor, so avoid those.
    file(STRINGS "${_hdr_file}" lua_version_strings
         REGEX "^#define[ \t]+LUA_(RELEASE[ \t]+\"Lua [0-9]|VERSION([ \t]+\"Lua [0-9]|_[MR])).*")

    string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_MAJOR[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUA_VERSION_MAJOR ";${lua_version_strings};")
    if(LUA_VERSION_MAJOR MATCHES "^[0-9]+$")
        string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_MINOR[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUA_VERSION_MINOR ";${lua_version_strings};")
        string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_RELEASE[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUA_VERSION_PATCH ";${lua_version_strings};")
        set(LUA_VERSION_STRING "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_PATCH}")
    else()
        string(REGEX REPLACE ".*;#define[ \t]+LUA_RELEASE[ \t]+\"Lua ([0-9.]+)\"[ \t]*;.*" "\\1" LUA_VERSION_STRING ";${lua_version_strings};")
        if(NOT LUA_VERSION_STRING MATCHES "^[0-9.]+$")
            string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION[ \t]+\"Lua ([0-9.]+)\"[ \t]*;.*" "\\1" LUA_VERSION_STRING ";${lua_version_strings};")
        endif()
        string(REGEX REPLACE "^([0-9]+)\\.[0-9.]*$" "\\1" LUA_VERSION_MAJOR "${LUA_VERSION_STRING}")
        string(REGEX REPLACE "^[0-9]+\\.([0-9]+)[0-9.]*$" "\\1" LUA_VERSION_MINOR "${LUA_VERSION_STRING}")
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]).*" "\\1" LUA_VERSION_PATCH "${LUA_VERSION_STRING}")
    endif()
    foreach(ver IN LISTS _lua_considered_versions)
        if(ver STREQUAL "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}")
            set(LUA_VERSION_MAJOR ${LUA_VERSION_MAJOR} PARENT_SCOPE)
            set(LUA_VERSION_MINOR ${LUA_VERSION_MINOR} PARENT_SCOPE)
            set(LUA_VERSION_PATCH ${LUA_VERSION_PATCH} PARENT_SCOPE)
            set(LUA_VERSION_STRING ${LUA_VERSION_STRING} PARENT_SCOPE)
            return()
        endif()
    endforeach()
endfunction()

function(_lua_find_header)
    _lua_get_header_dir_candidates()

    # Initialize as local variable
    set(CMAKE_IGNORE_PATH ${CMAKE_IGNORE_PATH})
	_lua_debug_msg("Considering following subdirs for lua.h: ${_lua_include_subdirs}")
    while(TRUE)
        if(NOT LUA_INCLUDE_DIR)
            # Find the next header to test. Check each possible subdir in order
            # This prefers e.g. higher versions as they are earlier in the list
            # It is also consistent with previous versions of FindLua
            foreach(subdir IN LISTS _lua_include_subdirs)
                find_path(LUA_INCLUDE_SUBDIR ${subdir}/lua.h
                    HINTS ENV LUA_DIR
                    PATHS ${_lua_additional_paths}
                )
                if(LUA_INCLUDE_SUBDIR)
                    # Find the real path taking the cache variable in account
                    find_path(LUA_INCLUDE_DIR lua.h
                        PATHS ${LUA_INCLUDE_SUBDIR}/${subdir}
                        NO_DEFAULT_PATH
                    )
                    unset(LUA_INCLUDE_SUBDIR CACHE)
                    unset(LUA_INCLUDE_SUBDIR)
                    break()
                endif()
            endforeach()
        endif()
        # fallback if not found yet
        find_path(LUA_INCLUDE_DIR lua.h
            HINTS ENV LUA_DIR
            PATHS ${_lua_additional_paths}
        )
        # Did not found header -> Fail
        if(NOT LUA_INCLUDE_DIR)
            return()
        endif()
        _lua_get_header_version()
        # Found accepted version -> Ok
        if(LUA_VERSION_STRING)
            _lua_debug_msg("Found suitable version ${LUA_VERSION_STRING} in ${LUA_INCLUDE_DIR}/lua.h")
            return()
        endif()
        # Found wrong version -> Ignore this path and retry
        _lua_debug_msg("Ignoring unsuitable version in ${LUA_INCLUDE_DIR}")
        list(APPEND CMAKE_IGNORE_PATH "${LUA_INCLUDE_DIR}")
        unset(LUA_INCLUDE_DIR CACHE)
        unset(LUA_INCLUDE_DIR)
        set(LUA_INCLUDE_DIR "" PARENT_SCOPE)
    endwhile()
endfunction()

_lua_get_versions()
_lua_find_header()
_lua_get_header_version()
unset(_lua_considered_versions)

if(LUA_VERSION_STRING)
    set(_lua_library_names
        lua${LUA_VERSION_MAJOR}${LUA_VERSION_MINOR}
        lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua-${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua.${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
		lua
    )

	_lua_debug_msg("Searching for lua library with base names: ${_lua_library_names}")

	find_library(LUA_LIBRARY
	  NAMES ${_lua_library_names} NAMES_PER_DIR
	  HINTS ENV LUA_DIR
	  PATH_SUFFIXES lib
	  PATHS ${_lua_additional_paths}
	)
	unset(_lua_library_names)
endif()

if(LUA_LIBRARY)
    # include the math library for Unix
    if(UNIX AND NOT APPLE AND NOT BEOS)
        find_library(LUA_MATH_LIBRARY m)
        set(LUA_LIBRARIES "${LUA_LIBRARY};${LUA_MATH_LIBRARY}")

        # include dl library for statically-linked Lua library
        get_filename_component(LUA_LIB_EXT ${LUA_LIBRARY} EXT)
        if(LUA_LIB_EXT STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX)
          list(APPEND LUA_LIBRARIES ${CMAKE_DL_LIBS})
        endif()

    # For Windows and Mac, don't need to explicitly include the math library
    else()
        set(LUA_LIBRARIES "${LUA_LIBRARY}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua
                                  REQUIRED_VARS LUA_LIBRARY LUA_LIBRARIES LUA_INCLUDE_DIR
                                  VERSION_VAR LUA_VERSION_STRING)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARY LUA_MATH_LIBRARY)
