# Use/Keep a file depending on condition
function(conditional_src listVar cond nameTrue nameFalse)
    if(${cond})
        set(srcToRemove ${nameFalse})
    else()
        set(srcToRemove ${nameTrue})
    endif()
    list(REMOVE_ITEM ${listVar} ${CMAKE_CURRENT_SOURCE_DIR}/${srcToRemove})
    set(${listVar} ${${listVar}} PARENT_SCOPE)
endfunction()

add_subdirectory(common)
add_subdirectory(log)
add_subdirectory(network)
