# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckAndAddWarnings)

# Add a compile option if it is supported
# Allows passing the language (C, CXX) as an optional second parameter
function(CheckAndAddFlag flag)
  if(ARGC GREATER_EQUAL 2)
    set(lang ${ARGV1})
  else()
    set(lang all)
  endif()
  make_flag_for_check(${flag} checkedFlag)
  # Remove special chars
  string(MAKE_C_IDENTIFIER ${checkedFlag} VarName)
  if(lang STREQUAL "C")
    set(CXX_FLAG_${VarName}_SUPPORTED FALSE)
  else()
    check_cxx_compiler_flag(${checkedFlag} CXX_FLAG_${VarName}_SUPPORTED)
  endif()
  if(lang STREQUAL "CXX")
    set(C_FLAG_${VarName}_SUPPORTED FALSE)
  else()
    check_c_compiler_flag(${checkedFlag} C_FLAG_${VarName}_SUPPORTED)
  endif()
  if(CXX_FLAG_${VarName}_SUPPORTED AND C_FLAG_${VarName}_SUPPORTED)
    add_compile_options(${flag})
  elseif(CXX_FLAG_${VarName}_SUPPORTED)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${flag}>)
  elseif(C_FLAG_${VarName}_SUPPORTED)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:${flag}>)
  endif()
endfunction()

function(CheckAndAddFlags)
  foreach(flag ${ARGN})
    CheckAndAddFlag(${flag})
  endforeach()
endfunction()
