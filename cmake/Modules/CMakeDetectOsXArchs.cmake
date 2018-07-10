# Detects all supported OSX architectures by reading libSystem.B.dylib
# Sets the possible values in OSX_DETECTED_ARCHS
# If a NO{ARCH} variable is set, then {ARCH} will be ignored

function(DetectOsXArchs)
    find_library(LIB_SYSTEM_PATH NAMES libSystem.B.tbd libSystem.B.dylib)
   	IF(NOT LIB_SYSTEM_PATH OR NOT EXISTS "${LIB_SYSTEM_PATH}")
		MESSAGE(FATAL_ERROR "No libSystem.B.* found while trying to detect the architecture")
    endif()
    MESSAGE(STATUS "Checking ${LIB_SYSTEM_PATH} for possible architectures")
    
    get_filename_component(LIBSYSTEM_EXT "${LIB_SYSTEM_PATH}" EXT)
    if("${LIBSYSTEM_EXT}" STREQUAL "B.dylib")
        # find lipo
        set(LIPO_NAMES apple-lipo lipo)
        if(${CMAKE_CXX_COMPILER} MATCHES "^(.*)-[^-]+$" OR ${CMAKE_C_COMPILER} MATCHES "^(.*)-[^-]+$")
            # Prefer lipo that has the same prefix as the compiler
            list(INSERT LIPO_NAMES 0 "${CMAKE_MATCH_1}-lipo")
        endif()
		FIND_PROGRAM(LIPO NAMES ${LIPO_NAMES})
		# read supported platforms	
		EXECUTE_PROCESS(
			COMMAND ${CMAKE_LIPO} "-info" "${LIB_SYSTEM_PATH}"
			RESULT_VARIABLE LIPO_RESULT
			ERROR_VARIABLE LIPO_ERROR
			OUTPUT_VARIABLE LIPO_OUTPUT
		)
	ELSE()
		# Newer OSX have only a text file *.tbd instead of the dylib file
        # Reuse the LIPO_ variables
		EXECUTE_PROCESS(
			COMMAND "grep" "archs" "${LIB_SYSTEM_PATH}"
			RESULT_VARIABLE LIPO_RESULT
			ERROR_VARIABLE LIPO_ERROR
			OUTPUT_VARIABLE LIPO_OUTPUT
		)
	ENDIF()
    
    if(NOT ${LIPO_RESULT} EQUAL 0)
        message(FATAL_ERROR "Lipo/Grep command failed with status ${LIPO_RESULT} and output: ${LIPO_ERROR}")
    endif()

    set(OSX_POSSIBLE_ARCHS x86_64 i386 i686 ppc)
	set(OSX_DETECTED_ARCHS)
    foreach(arch IN LISTS OSX_POSSIBLE_ARCHS)
        IF(${LIPO_OUTPUT} MATCHES ${arch})
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
