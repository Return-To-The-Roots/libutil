# In Windows adds all dlls passed to this function if they exist. Converts to absolute path if relative path is given
function(gather_dll_add)
    foreach(curFile ${ARGN})
        get_filename_component(absCurFile ${curFile} ABSOLUTE)
        if(EXISTS ${absCurFile} AND NOT IS_DIRECTORY ${absCurFile})
            list(APPEND GATHER_DLLS ${absCurFile})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES GATHER_DLLS)
    set(GATHER_DLLS ${GATHER_DLLS} CACHE INTERNAL "")
endfunction()

# On Windows this adds the dll to the dlls to copy if it can be found.
# Sets <lib_prefix>_DLL to the path of the dll
function(gather_dll_by_name lib_prefix dll_name)
    if(WIN32)
        find_program(${lib_prefix}_DLL ${dll_name})
        if(${lib_prefix}_DLL)
            gather_dll_add(${${lib_prefix}_DLL})
        endif()
    endif()
endfunction()

# On Windows this tries to find the dll name from the static library and then the dll using find_program
# If found it appends it to the dlls to copy
function(gather_dll lib_prefix)
    # Dlls are windows only
    if(WIN32)
        # If the library is a dll we take it
        if("${${lib_prefix}_LIBRARY}" MATCHES "^[^;]*\\.dll$")
            gather_dll_by_name(${lib_prefix} ${${lib_prefix}_LIBRARY})
        elseif("${${lib_prefix}_LIBRARY}" MATCHES "^[^;]*\\.(lib|a)$")
            # Find a dll name
            set(DLL_REGEX "[a-zA-Z0-9_-]*\\.dll")
            file(STRINGS ${${lib_prefix}_LIBRARY} ${lib_prefix}_DLL_NAME_RAW LIMIT_COUNT 1 REGEX ${DLL_REGEX})
            if(NOT "${${lib_prefix}_DLL_NAME_RAW}" STREQUAL "")
                string(REGEX MATCH ${DLL_REGEX} ${lib_prefix}_DLL_NAME "${${lib_prefix}_DLL_NAME_RAW}")
                gather_dll_by_name(${lib_prefix} ${${lib_prefix}_DLL_NAME})
            endif()
        endif()
    endif()
endfunction()

# On Windows this adds a post-build command to the given target that will copy all gathered dlls to the directory containing the output file
function(gather_dll_copy target)
    if(WIN32 AND GATHER_DLLS AND NOT CMAKE_CROSSCOMPILING)
        # Just in case...
        list(REMOVE_DUPLICATES GATHER_DLLS)
        set(_missing "")
        foreach(file_i ${GATHER_DLLS})
            if(EXISTS ${file_i})
                add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${file_i} $<TARGET_FILE_DIR:${target}>)
            else()
                list(APPEND _missing "${file_i}")
            endif()
        endforeach()
        if(_missing)
            message(WARNING "Could not find the following files: ${_missing}")
        endif()
    endif()
endfunction()
