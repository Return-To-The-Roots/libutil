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
    check_and_add_cxx_warnings(${target} ${visibility}
      -Wpedantic
      -Wparentheses
      -Wno-error=type-limits
      -Wfloat-conversion
      -Wno-long-long
      -Wno-deprecated-register
      -Wno-unknown-pragmas
      -fno-strict-aliasing
      -Qunused-arguments
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
