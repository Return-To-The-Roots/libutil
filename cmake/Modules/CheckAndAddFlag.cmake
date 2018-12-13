include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

macro(make_flag_for_check flag outVar)
	# We cannot check for -Wno-foo as this won't throw a warning so we must check for the -Wfoo option directly
	# http://stackoverflow.com/questions/38785168/cc1plus-unrecognized-command-line-option-warning-on-any-other-warning
	string(REGEX REPLACE "^-Wno-" "-W" ${outVar} ${flag})
endmacro()

function(check_cxx_warning warningFlag var)
	make_flag_for_check(${warningFlag} escapedFlag)
    # Remove special chars
    string(MAKE_C_IDENTIFIER CXX_WARNING_${escapedFlag}_SUPPORTED VarName)
	check_cxx_compiler_flag(${escapedFlag} ${VarName})
	set(${var} ${${VarName}} PARENT_SCOPE)
endfunction()

function(check_and_add_cxx_warning target visibility warningFlag)
	check_cxx_warning(${warningFlag} supported)
	if(supported)
		target_compile_options(${target} ${visibility} ${warningFlag})
	endif()
endfunction()

function(check_and_add_cxx_warnings target visibility)
    foreach(warningFlag ${ARGN})
        check_and_add_cxx_warning(${target} ${visibility} ${warningFlag})
    endforeach()
endfunction()

function(CheckAndAddFlag flag)
	make_flag_for_check(${flag} checkedFlag)
    # Remove special chars
    string(MAKE_C_IDENTIFIER ${checkedFlag} VarName)
	check_cxx_compiler_flag(${checkedFlag} CXX_FLAG_${VarName}_SUPPORTED)
	check_c_compiler_flag(${checkedFlag} C_FLAG_${VarName}_SUPPORTED)
	if(CXX_FLAG_${VarName}_SUPPORTED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
	endif()
	if(C_FLAG_${VarName}_SUPPORTED)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
	endif()
endfunction()

function(CheckAndAddFlags)
    foreach(flag ${ARGN})
        CheckAndAddFlag(${flag})
    endforeach()
endfunction()
