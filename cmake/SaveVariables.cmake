# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# Save the variables passed as arguments and pushes their current values to a stack
# Restore all those with pop_saved_variables()
macro(save_variables)
  if(NOT DEFINED _RTTR_SAVE_VARIABLES_COUNTER)
    set(_RTTR_SAVE_VARIABLES_COUNTER 0)
  endif()

  math(EXPR _RTTR_SAVE_VARIABLES_COUNTER "${_RTTR_SAVE_VARIABLES_COUNTER}+1")
  set(_RTTR_SAVE_VARIABLES_NAMES_${_RTTR_SAVE_VARIABLES_COUNTER} "")

  foreach(var IN ITEMS ${ARGN})
    set(_RTTR_SAVE_VARIABLES_OLD_${_RTTR_SAVE_VARIABLES_COUNTER}_${var} "${${var}}")
    list(APPEND _RTTR_SAVE_VARIABLES_NAMES_${_RTTR_SAVE_VARIABLES_COUNTER} "${var}")
  endforeach()
endmacro()

# Restore last recently saved variables
macro(pop_saved_variables)
  if(NOT _RTTR_SAVE_VARIABLES_COUNTER)
    message(FATAL_ERROR "No saved variables")
  endif()
  foreach(var IN LISTS _RTTR_SAVE_VARIABLES_NAMES_${_RTTR_SAVE_VARIABLES_COUNTER})
    set(${var} ${_RTTR_SAVE_VARIABLES_OLD_${_RTTR_SAVE_VARIABLES_COUNTER}_${var}})
  endforeach()
  math(EXPR _RTTR_SAVE_VARIABLES_COUNTER "${_RTTR_SAVE_VARIABLES_COUNTER}-1")
endmacro()
