cmake_minimum_required(VERSION 3.10)
project(commander)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE -O3)

find_package(GTest REQUIRED)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    .
)

add_executable(lexerTests source/lexer/lexer.cpp tests/lexer_tests.cpp)

target_link_libraries(lexerTests ${GTEST_BOTH_LIBRARIES})