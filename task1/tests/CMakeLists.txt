cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(app_tests
    utils_tests.cpp
)

target_link_libraries(app_tests PRIVATE
    app_core
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(app_tests)