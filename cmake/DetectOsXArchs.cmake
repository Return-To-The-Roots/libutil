# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# Detects all supported OSX architectures by reading libSystem.B.dylib
# Sets the possible values in OSX_DETECTED_ARCHS
# If a NO{ARCH} variable is set, then {ARCH} will be ignored

function(detect_osx_archs)
    find_library(LIB_SYSTEM_PATH NAMES libSystem.B.tbd libSystem.B.dylib)
    if(NOT LIB_SYSTEM_PATH OR NOT EXISTS "${LIB_SYSTEM_PATH}")
        message(FATAL_ERROR "No libSystem.B.* found while trying to detect the architecture")
    endif()
    message(STATUS "Checking ${LIB_SYSTEM_PATH} for possible architectures")

    get_filename_component(LIBSYSTEM_EXT "${LIB_SYSTEM_PATH}" EXT)
    if(LIBSYSTEM_EXT STREQUAL ".B.dylib")
        # find lipo
        set(LIPO_NAMES apple-lipo lipo)
        if(CMAKE_CXX_COMPILER MATCHES "^(.*)-[^-]+$" OR CMAKE_C_COMPILER MATCHES "^(.*)-[^-]+$")
            # Prefer lipo that has the same prefix as the compiler
            list(INSERT LIPO_NAMES 0 "${CMAKE_MATCH_1}-lipo")
        endif()
        find_program(LIPO NAMES ${LIPO_NAMES})
        set(cmd "${LIPO}")
        set(args -info)
    else()
        # Newer OSX have only a text file *.tbd instead of the dylib file
        set(cmd grep)
        set(args archs)
    endif()

    # read supported platforms
    execute_process(
        COMMAND "${cmd}" ${args} "${LIB_SYSTEM_PATH}"
        RESULT_VARIABLE cmdResult
        ERROR_VARIABLE cmdStdErr
        OUTPUT_VARIABLE cmdStdOut
    )
    if(NOT cmdResult EQUAL 0 AND cmd STREQUAL "grep")
        # Try again for newer XCode (seen on version 12)
        execute_process(
            COMMAND grep target "${LIB_SYSTEM_PATH}"
            RESULT_VARIABLE cmdResult
            ERROR_VARIABLE cmdStdErr
            OUTPUT_VARIABLE cmdStdOut
        )
    endif()
    if(NOT cmdResult EQUAL 0)
        if(LIBSYSTEM_EXT STREQUAL ".B.tbd")
            file(READ "${LIB_SYSTEM_PATH}" tbdContents)
            set(tbdContents " TBD: ${tbdContents}")
        else()
            set(tbdContents "")
        endif()
        message(WARNING "Command '${cmd}' failed with status ${cmdResult} and output: ${cmdStdOut} ${cmdStdErr}"
                        "Assuming at least x86_64 is supported.${tbdContents}")
        set(cmdStdOut x86_64)
    endif()

    set(OSX_POSSIBLE_ARCHS x86_64 i386 i686 ppc)
    set(OSX_DETECTED_ARCHS)
    foreach(arch IN LISTS OSX_POSSIBLE_ARCHS)
        if(cmdStdOut MATCHES "${arch}")
            list(APPEND OSX_DETECTED_ARCHS ${arch})
        endif()
    endforeach()

    if(NOT OSX_DETECTED_ARCHS)
        message(FATAL_ERROR "woooops: this should not happen! your system has no known architecture!")
    endif()

    message(STATUS "Possible architectures: ${OSX_DETECTED_ARCHS}")
    set(OSX_POSSIBLE_ARCHS ${OSX_POSSIBLE_ARCHS} PARENT_SCOPE)
    set(OSX_DETECTED_ARCHS ${OSX_DETECTED_ARCHS} PARENT_SCOPE)
endfunction()
