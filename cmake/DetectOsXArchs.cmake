# Detects all supported OSX architectures by reading libSystem.B.dylib
# Sets the possible values in OSX_DETECTED_ARCHS
# If a NO{ARCH} variable is set, then {ARCH} will be ignored

function(detect_osx_archs)
    find_library(LIB_SYSTEM_PATH NAMES libSystem.B.tbd libSystem.B.dylib)
   	IF(NOT LIB_SYSTEM_PATH OR NOT EXISTS "${LIB_SYSTEM_PATH}")
		MESSAGE(FATAL_ERROR "No libSystem.B.* found while trying to detect the architecture")
    endif()
    MESSAGE(STATUS "Checking ${LIB_SYSTEM_PATH} for possible architectures")
    
    get_filename_component(LIBSYSTEM_EXT "${LIB_SYSTEM_PATH}" EXT)
    if(LIBSYSTEM_EXT STREQUAL ".B.dylib")
        # find lipo
        set(LIPO_NAMES apple-lipo lipo)
        if(${CMAKE_CXX_COMPILER} MATCHES "^(.*)-[^-]+$" OR ${CMAKE_C_COMPILER} MATCHES "^(.*)-[^-]+$")
            # Prefer lipo that has the same prefix as the compiler
            list(INSERT LIPO_NAMES 0 "${CMAKE_MATCH_1}-lipo")
        endif()
		FIND_PROGRAM(LIPO NAMES ${LIPO_NAMES})
        set(_cmd "${LIPO}")
        set(_args -info)
	ELSE()
		# Newer OSX have only a text file *.tbd instead of the dylib file
        set(_cmd grep)
        set(_args archs)
	ENDIF()
    
    # read supported platforms	
    EXECUTE_PROCESS(
        COMMAND "${_cmd}" ${_args} "${LIB_SYSTEM_PATH}"
        RESULT_VARIABLE _RESULT
        ERROR_VARIABLE _ERROR
        OUTPUT_VARIABLE _OUTPUT
    )
    if(NOT _RESULT EQUAL 0)
        message(FATAL_ERROR "Command '${_cmd}' failed with status ${_RESULT} and output: ${_ERROR}")
    endif()

    set(OSX_POSSIBLE_ARCHS x86_64 i386 i686 ppc)
	set(OSX_DETECTED_ARCHS)
    foreach(arch IN LISTS OSX_POSSIBLE_ARCHS)
        IF(${_OUTPUT} MATCHES ${arch})
            list(APPEND OSX_DETECTED_ARCHS ${arch})
        ENDIF()
    endforeach()
	
	IF(NOT OSX_DETECTED_ARCHS)
		MESSAGE(FATAL_ERROR "woooops: this should not happen! your system has no known architecture!")
	ENDIF()

	MESSAGE(STATUS "Possible architectures: ${OSX_DETECTED_ARCHS}")
    set(OSX_POSSIBLE_ARCHS ${OSX_POSSIBLE_ARCHS} PARENT_SCOPE)
    set(OSX_DETECTED_ARCHS ${OSX_DETECTED_ARCHS} PARENT_SCOPE)
endfunction()
