cmake_minimum_required(VERSION 3.16)

project(simpleparser)

set(CMAKE_CXX_STANDARD 17)

add_library(
    simpleparser
    src/simple_parser.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    parser_test
    tests/parser_test.cpp
)
target_link_libraries(
    parser_test
    simpleparser
    GTest::gtest_main
)
target_include_directories(
    parser_test
    PUBLIC src
)
include(GoogleTest)
gtest_discover_tests(parser_test)
