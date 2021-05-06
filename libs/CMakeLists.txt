# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

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
