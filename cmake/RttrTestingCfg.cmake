# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# we do not use CTest because it will pull in a lot of unneeded targets
# include(CTest)

if(NOT DEFINED RTTR_TESTING_CFG)
    set(RTTR_TESTING_CFG 1)

    option(BUILD_TESTING "Build tests" ON)
    if(BUILD_TESTING)
        message(STATUS "Testing ('make test') is enabled. Add -DBUILD_TESTING=OFF to disable tests.")
        enable_testing()
    else()
        message(STATUS "Testing ('make test') is disabled. Add -DBUILD_TESTING=ON to enable tests.")
    endif()
endif()
