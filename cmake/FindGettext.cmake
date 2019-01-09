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

FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)

FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

function(GETTEXT_CREATE_TRANSLATIONS _potFile)
   SET(_gmoFiles)
   GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   set(options ALL)
   set(oneValueArgs DESTINATION)
   set(multiValueArgs)
   cmake_parse_arguments(_poFiles "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

   SET(_addToAll)
   IF(_poFiles_ALL)
      SET(_addToAll "ALL")
   ENDIF()

   FOREACH (_currentPoFile ${_poFiles_UNPARSED_ARGUMENTS})
      GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
      GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
      GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
      file(RELATIVE_PATH _rel_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${_abs_PATH})

      if(_poFiles_DESTINATION)
         SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_poFiles_DESTINATION}/${_lang}.mo)
      else()
         SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}/${_lang}.mo)
      endif()

      ADD_CUSTOM_COMMAND( 
         OUTPUT ${_gmoFile} 
         COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --sort-output --no-wrap --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
         DEPENDS ${_absPotFile} ${_absFile} 
      )

      if(_poFiles_DESTINATION)
         install(FILES ${_gmoFile} DESTINATION ${_poFiles_DESTINATION})
      endif()
      SET(_gmoFiles ${_gmoFiles} ${_gmoFile})

   ENDFOREACH (_currentPoFile )

   ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles})

endfunction()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gettext DEFAULT_MSG GETTEXT_MSGMERGE_EXECUTABLE GETTEXT_MSGFMT_EXECUTABLE)
