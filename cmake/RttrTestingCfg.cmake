# we do not use CTest because it will pull in a lot of unneeded targets
# include(CTest)

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    message(STATUS "Testing ('make test') is enabled. Add -DBUILD_TESTING=OFF to disable tests.")
    enable_testing()
else()
    message(STATUS "Testing ('make test') is disabled. Add -DBUILD_TESTING=ON to enable tests.")
endif()
