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
      /w34389 # Signed/Unsigned operations
      /wd4250 # 'class1' : inherits 'class2::member' via dominance (virtual inheritance related)
      /wd4127 # conditional expr is constant
      /wd4100 # unreferenced formal parameter
      /wd4512 # assignment operator could not be created
      /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
      /wd4250 # "use secure function"
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
    include(CheckAndAddFlag)
    # Additional options besides -Wall, -Wextra, -pedantic
    # Keep those sorted to check for uniqueness
    check_and_add_cxx_warnings(${target} ${visibility}
      -fno-strict-aliasing
      #-pedantic-errors
      -Qunused-arguments
      #-Wcast-align
      #-Wcast-qual
      #-Wconversion
      #-Wctor-dtor-privacy
      #-Wdisabled-optimization
      -Wfloat-conversion
      #-Wfloat-equal
      #-Wformat-nonliteral
      #-Wformat-security
      #-Wformat=2
      #-Wimplicit
      #-Wimport
      #-Winit-self
      #-Winline
      #-Winvalid-pch
      #-Wlogical-op
      #-Wmissing-declarations
      #-Wmissing-field-initializers
      #-Wmissing-format-attribute
      #-Wmissing-include-dirs
      #-Wmissing-noreturn
      -Wno-unknown-pragmas
      #-Wnoexcept
      #-Wold-style-cast
      #-Woverloaded-virtual
      #-Wpacked
      #-Wpadded
      -Wparentheses
      -Wpedantic
      #-Wpointer-arith
      #-Wredundant-decls
      #-Wshadow
      #-Wsign-conversion
      #-Wsign-promo
      #-Wstack-protector
      #-Wstrict-aliasing=2
      #-Wstrict-null-sentinel
      #-Wstrict-overflow=5
      #-Wswitch-default
      #-Wswitch-enum
      #-Wundef
      #-Wunreachable-code
      #-Wunused
      #-Wunused-parameter
      #-Wvariadic-macros
      #-Wwrite-strings
    )

    check_cxx_warning(-Wsuggest-override supported)
    if(supported)
      target_compile_options(${target} ${visibility} "$<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override;-Wno-error=suggest-override>")
    endif()
    check_cxx_warning(-Winconsistent-missing-override supported)
    if(supported)
      target_compile_options(${target} ${visibility} "$<$<COMPILE_LANGUAGE:CXX>:-Wno-error=inconsistent-missing-override>")
    endif()
  endif()
endfunction()
