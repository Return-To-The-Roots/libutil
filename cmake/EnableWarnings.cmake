# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

option(RTTR_ENABLE_WERROR "Build with warnings turned into errors" ON)
function(enable_warnings target)
  get_target_property(targetType ${target} TYPE)
  if(targetType STREQUAL "INTERFACE_LIBRARY")
    set(visibility INTERFACE)
  else()
    set(visibility PUBLIC)
  endif()
  if(MSVC)
    target_compile_options(${target} ${visibility}
      /W3
      /MP # parallel compilation
      /w34062 # Enum not handled in switch
      /w34388 # Signed/Unsigned operations (undocumented)
      /w34389 # Signed/Unsigned operations
      /wd4127 # conditional expr is constant
      /wd4250 # 'class1' : inherits 'class2::member' via dominance (virtual inheritance related)
      /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
      /wd4373 # override where const in parameters differs handled differently in pre VS2008
      /wd4512 # assignment operator could not be created
    )

    target_compile_definitions(${target} ${visibility}
      _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS # disable "use secure function"
      _CRT_NONSTDC_NO_DEPRECATE # disable MSVC posix functions
      -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )
    if(RTTR_ENABLE_WERROR)
      target_compile_options(${target} ${visibility} /WX) # warning = error
    endif()
  else()
    target_compile_options(${target} ${visibility} -Wall -pedantic -Wextra)
    if(RTTR_ENABLE_WERROR)
      target_compile_options(${target} ${visibility} -Werror)
    endif()
    include(CheckAndAddWarnings)
    # Additional options besides -Wall, -Wextra, -pedantic
    # Keep those sorted to check for uniqueness
    check_and_add_warnings(TARGET ${target} VISIBILITY ${visibility}
      ALL
        -Wno-unused-command-line-argument
      CXX
        -fno-strict-aliasing
        -pedantic-errors
        -Qunused-arguments
        -Wcast-qual
        #-Wconversion
        -Wdisabled-optimization
        -Wfloat-conversion
        -Wformat-nonliteral
        -Wformat-security
        -Wformat=2
        -Wimport
        -Winit-self
        #-Winline
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-field-initializers
        -Wmissing-format-attribute
        -Wmissing-include-dirs
        -Wmissing-noreturn
        #-Wold-style-cast
        -Wpacked
        -Wparentheses
        -Wpedantic
        -Wpointer-arith
        #-Wshadow
        #-Wsign-conversion # Lot's of work, maybe later?
        -Wstrict-aliasing=2
        -Wundef
        -Wunused
        -Wunused-parameter
        -Wwrite-strings
        -Wno-unknown-pragmas
        -Wctor-dtor-privacy
        -Wnoexcept
        -Woverloaded-virtual
        -Wstrict-null-sentinel
        -Wno-maybe-uninitialized # False positives e.g. with variant/optional
        -Wno-error=inconsistent-missing-override
    )
    if(NOT WIN32)
      # Lot's of false-positives on windows.
      # E.g. __builtin_ia32_crc32qi on MinGW and SDL
      check_and_add_warnings(TARGET ${target} VISIBILITY ${visibility} CXX -Wredundant-decls)
    endif()
    # Prior to 9.2 "final" was not considered "override"
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.2)
      target_compile_options(${target} ${visibility} $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override -Wno-error=suggest-override>)
    endif()
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      # Lot's of false-positives on for e.g. strcmp
      check_and_add_warnings(TARGET ${target} VISIBILITY ${visibility} CXX -Wunreachable-code)
    elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
      # Wrong warnings for std::array until clang 6
      target_compile_options(${target} ${visibility} -Wno-missing-braces)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0))
      # To many false positives until clang 10 fixed them
      check_and_add_warnings(TARGET ${target} VISIBILITY ${visibility} CXX -Wno-range-loop-analysis)
    endif()
    # Not used:
      #-Wpadded
      #-Wstack-protector
      #-Wswitch-default
      #-Wswitch-enum
      #-Wfloat-equal
      #-Wcast-align
      #-Wstrict-overflow=2 # To many false positives
      #-Wsign-promo # std::to_string(uint8_t{42}) should work
  endif()
endfunction()
