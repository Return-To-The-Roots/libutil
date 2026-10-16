# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# - Try to find Iconv 
# Once done this will define 
# 
#  ICONV_FOUND - system has Iconv 
#  ICONV_INCLUDE_DIR - the Iconv include directory 
#  ICONV_LIBRARY - Link these to use Iconv 
#  Iconv::Iconv - imported target (as created by CMake's own FindIconv, required e.g. by Boost.Locale's CMake config)
# 

SET(ICONV_DIR_SEARCH $ENV{ICONV_ROOT})

FIND_PATH(ICONV_INCLUDE_DIR iconv.h 
  ${ICONV_DIR_SEARCH}/include	
  /usr/include 
  /usr/local/include 
) 

FIND_LIBRARY(ICONV_LIBRARY NAMES iconv libiconv2 c
  PATHS 
  ${ICONV_DIR_SEARCH}/lib
  /usr/${LIB_DESTINATION}
  /usr/local/${LIB_DESTINATION}) 
 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iconv DEFAULT_MSG ICONV_INCLUDE_DIR ICONV_LIBRARY)

MARK_AS_ADVANCED(
  ICONV_INCLUDE_DIR
  ICONV_LIBRARY
)

if(Iconv_FOUND AND NOT TARGET Iconv::Iconv)
  add_library(Iconv::Iconv INTERFACE IMPORTED)
  set_target_properties(Iconv::Iconv PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ICONV_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${ICONV_LIBRARY}")
endif()
