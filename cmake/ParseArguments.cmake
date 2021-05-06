# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

macro(parse_arguments options one_value_keywords multi_value_keywords required_options)
    cmake_parse_arguments(ARG "${options}" "${one_value_keywords}" "${multi_value_keywords}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    foreach(required IN LISTS required_options)
        if(NOT ARG_${required})
            message (FATAL_ERROR "Required argument ${required} is missing")
        endif()
    endforeach()
endmacro()
