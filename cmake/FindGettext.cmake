# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# - Find GNU gettext tools
# This module looks for the GNU gettext tools. This module defines the
# following values:
#  GETTEXT_MSGMERGE_EXECUTABLE: the full path to the msgmerge tool.
#  GETTEXT_MSGFMT_EXECUTABLE: the full path to the msgfmt tool.
#  GETTEXT_FOUND: True if gettext has been found.
#
# Additionally it provides the following macros:
# GETTEXT_CREATE_TRANSLATIONS ( outputFile [ALL] file1 ... fileN )
#    This will create a target "translations" which will convert the
#    given input po files into the binary output mo file. If the
#    ALL option is used, the translations will also be created when
#    building the default target.

#
# This file has been modified. Do not use vanilla FindGettext.cmake
#

find_program(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gettext DEFAULT_MSG GETTEXT_MSGMERGE_EXECUTABLE GETTEXT_MSGFMT_EXECUTABLE)

if(Gettext_FOUND)
    function(gettext_create_translations potFile)
        get_filename_component(potBasename ${potFile} NAME_WE)
        get_filename_component(absPotFile ${potFile} ABSOLUTE)

        set(options ALL)
        set(oneValueArgs DESTINATION INSTALL_DESTINATION)
        set(multiValueArgs FILES)
        cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        foreach(reqArg DESTINATION FILES)
            if(NOT ARG_${reqArg})
                message(FATAL_ERROR "Missing argument ${reqArg}")
            endif()
        endforeach()
        if(ARG_UNPARSED_ARGUMENTS)
            message(FATAL_ERROR "Unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
        endif()
        if(IS_ABSOLUTE ${ARG_DESTINATION})
            set(destination ${ARG_DESTINATION})
        else()
            set(destination ${CMAKE_CURRENT_BINARY_DIR}/${ARG_DESTINATION})
        endif()
        file(MAKE_DIRECTORY ${destination})

        set(moFiles "")
        foreach (currentPoFile ${ARG_FILES})
            get_filename_component(absFile ${currentPoFile} ABSOLUTE)
            get_filename_component(lang ${absFile} NAME_WE)

            set(moFile ${destination}/${lang}.mo)

            add_custom_command(
              OUTPUT ${moFile}
              COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --sort-output --no-wrap --quiet --update --backup=none ${absFile} ${absPotFile}
              COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${moFile} ${absFile}
              DEPENDS ${absPotFile} ${absFile}
            )

            list(APPEND moFiles ${moFile})
       endforeach()

        if(ARG_ALL)
            set(addToAll "ALL")
        else()
            set(addToAll "")
        endif()
        add_custom_target(translations ${addToAll} DEPENDS ${moFiles})

        if(ARG_INSTALL_DESTINATION)
            install(FILES ${moFiles} DESTINATION ${ARG_INSTALL_DESTINATION})
        endif()
    endfunction()
endif()
