cmake_minimum_required(VERSION 3.14)
project(commander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE -O3)

find_package(GTest REQUIRED)

#  ===========================
#  ||  Main Build Configs:  ||
#  ===========================

#  Each build configuration will be made up of one or two lines.
#
#  Executables:
#  ------------
#
#      The main build configuration is an executable;
#      it's a fully-functioning program that can be invoked from the command line.
#
#      Form:
#
#      ```
#      add_executable([name] [sources])
#      target_link_libraries([name] [libraries])
#      ```
#
#      Here, `[name]` refers to the name of the executable,
#      `[sources]` refers to all source files used by the executable,
#      and `[libraries]` refers to all libraries used by the executable.
#      If the executable doesn't use any linked libraries,
#      then the second line may be omitted.
#
#      Examples:
#
#      ```
#      add_executable(commander source/commander/commander.cpp source/lexer/lexer.cpp source/parser/parser.cpp)
#      ```
#
#      ```
#      add_executable(commander source/commander/commander.cpp)
#      target_link_libraries(commander lexer parser)
#      ```
#
#  Libraries:
#  ----------
#
#      Because the main executable will involve a lot of source files,
#      it's useful to break up the different pieces of the application into
#      separately-linked libraries.
#
#      Form:
#
#      ```
#      add_library([name] [sources])
#      target_link_libraries([name] [libraries])
#      ```
#
#      Here, `[name]` refers to the name of the library,
#      `[sources]` refers to all source files used by the library,
#      and `[libraries]` refers to all other libraries used by the library.
#      If the library doesn't use any linked libraries,
#      then the second line may be omitted.
#
#      Examples:
#
#      ```
#      add_library(lexer source/lexer/lexer.cpp)
#      ```
#
#      ```
#      add_library(type-checker source/type-checker/type-checker.cpp)
#      target_link_libraries(type-checker token ast-node)
#      ```
#
#      Then, this library can be used by used in a `target_link_libraries` call
#      instead of specifying source files every time a part of the program needs to use it.
#
#      ```
#      add_executable(commander source/commander/commander.cpp)
#      target_link_libraries(commander lexer parser type-checker)
#      ```

#  =========================
#  ||  GoogleTest Setup:  ||
#  =========================

include(FetchContent)
FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)


#  ============================
#  ||  Test Configurations:  ||
#  ============================

#  Each test configuration will be made up of three lines:
#
#      ```
#      add_executable([name] [sources] [tests])
#      target_link_libraries([name] [libraries] GTest::gtest_main)
#      gtest_discover_tests([name])
#      ```
#
#  I use four variables here.
#    * `[name]` refers to the name of the test configuration (e.g., `lexerTests`).
#    * `[sources]` refers to all source C++ files for this unit (e.g., `source/lexer/lexer.cpp`).
#      (Can be empty if the source files are in a linked library in the line below.)
#    * `[libraries]` refers to all linked libraries for this unit.
#      This is useful when you're trying to test something for which you've already made a `add_library` configuration.
#      (Can be empty if all source files are specified in the the line above.)
#    * `[tests]` refers the file containing the unit tests (e.g., `tests/lexer_tests.cpp`).

find_package(GTest REQUIRED)

add_executable(lexerTests source/lexer/lexer.cpp source/util/commander_exception.cpp tests/lexer_tests.cpp)

target_link_libraries(lexerTests ${GTEST_BOTH_LIBRARIES})
