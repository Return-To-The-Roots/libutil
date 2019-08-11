if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  option(RTTR_ENABLE_SANITIZERS "Enable ASAN and UBSAN for compilers supporting this" OFF)

  if(RTTR_ENABLE_SANITIZERS)
    # Use ASAN and UBSAN, make it fail on any error, improve stack traces
    set(sanitizer_flags -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    list(APPEND sanitizer_flags -fsanitize-recover=enum,alignment)

    add_compile_options(${sanitizer_flags})
    string(REPLACE ";" " " sanitizer_flags "${sanitizer_flags}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " ${sanitizer_flags}")
    string(APPEND CMAKE_MODULE_LINKER_FLAGS " ${sanitizer_flags}")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " ${sanitizer_flags}")
    set(CMAKE_ENABLE_EXPORTS ON) # Required to resolve type_info-structs across executable-plugin-boundaries
  endif()
endif()

