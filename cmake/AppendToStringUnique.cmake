# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# Add all arguments to the string variable if they are not already contained
function(append_to_string_unique variable)
  set(newValue "${${variable}}")
	# Now loop over each required argument and see if it is in our
	# current list of arguments.
	foreach(arg IN LISTS ARGN)
    string(FIND " ${newValue} " " ${arg} " index)
    if(index EQUAL -1)
      string(APPEND newValue " ${arg}")
    endif()
  endforeach()
	SET(${variable} "${newValue}" PARENT_SCOPE)
endfunction()
