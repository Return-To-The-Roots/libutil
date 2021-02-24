if(NOT TARGET Boost::unit_test_framework)
    find_package(Boost 1.69 REQUIRED COMPONENTS unit_test_framework)
endif()

set(RTTR_UBSAN_SUPPRESSION_FILE ${CMAKE_CURRENT_LIST_DIR}/ubsan.supp)
set(RTTR_LSAN_SUPPRESSION_FILE ${CMAKE_CURRENT_LIST_DIR}/lsan.supp)

# Add a new test case for boost tests with working directory in the binary dir root
# Params:
# NAME <name>
# COST <cost>, defaults to 10
# LIBS/INCLUDES <value, value, ...>
# CONFIGURATIONS <value, value...>
function(add_testcase)
    set(options )
    set(oneValueArgs NAME COST)
    set(multiValueArgs LIBS INCLUDES CONFIGURATIONS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    string(MAKE_C_IDENTIFIER "Test_${ARG_NAME}" name)
    file(GLOB sources *.[ch]pp *.[hc] *.cxx)
    add_executable(${name} ${sources})
    target_link_libraries(${name} PRIVATE ${ARG_LIBS}
        PRIVATE Boost::unit_test_framework Boost::disable_autolinking
    )
    target_include_directories(${name} PRIVATE ${ARG_INCLUDES})
    # Heuristically guess if we are compiling against dynamic boost
    if(NOT Boost_USE_STATIC_LIBS AND Boost_UNIT_TEST_FRAMEWORK_LIBRARY AND NOT Boost_UNIT_TEST_FRAMEWORK_LIBRARY MATCHES "\\${CMAKE_STATIC_LIBRARY_SUFFIX}\$")
        target_compile_definitions(${name} PRIVATE BOOST_TEST_DYN_LINK)
    endif()
    if(MSVC)
        # Use full paths for defines (makes Boost.Test with the VS addin work better)
        target_compile_options(${name} PUBLIC /FC)
    endif()
    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR} CONFIGURATIONS ${ARG_CONFIGURATIONS})
    if(MSVC)
        set_property(TARGET ${name} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    endif()
    if(NOT DEFINED ARG_COST)
      set(ARG_COST 10)
    endif()
    set_tests_properties(${name} PROPERTIES COST "${ARG_COST}")
    if(RTTR_ENABLE_SANITIZERS)
        set(options
            ASAN_OPTIONS=detect_odr_violation=0
            UBSAN_OPTIONS=halt_on_error=1,print_stacktrace=1,suppressions=${RTTR_UBSAN_SUPPRESSION_FILE}
            LSAN_OPTIONS=halt_on_error=1,suppressions=${RTTR_LSAN_SUPPRESSION_FILE}
        )
        set_tests_properties(${name} PROPERTIES ENVIRONMENT "${options}")
    endif()
endfunction()
