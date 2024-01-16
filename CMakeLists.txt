#  ======================
#  ||  CMakeLists.txt  ||
#  ======================
#
#  Here, you'll find detailed documentation on how to set up new build configurations.
#
#
#  ==================================
#  ||  Main Build Configurations:  ||
#  ==================================
#
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
#      Example:
#
#      ```
#      add_executable(commander source/commander/commander.cpp)
#      target_link_libraries(commander lexer parser)
#      ```
#
#  Libraries:
#  ----------
#
#      Everything other than executables will be libraries.
#      This is because the program will involve a lot of source files,
#      so it's useful to break up the different pieces of the application into
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
#      add_library(typeChecker source/type_checker/type-checker.cpp)
#      target_link_libraries(typeChecker lexer parser)
#      ```
#
#      Then, this library can be used by used in a `target_link_libraries` call
#      instead of specifying source files every time a part of the program needs to use it.
#
#      ```
#      add_executable(commander source/commander/commander.cpp)
#      target_link_libraries(commander lexer parser type-checker)
#      ```
#
#
#  ============================
#  ||  Test Configurations:  ||
#  ============================
#
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


#  Project configuration:
#  ----------------------

cmake_minimum_required(VERSION 3.14)
project(commander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE -O3)

include_directories(.)


#  GoogleTest setup:
#  -----------------

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


#  Main build configurations:
#  --------------------------

add_library(lexer source/lexer/lexer.cpp source/util/commander_exception.cpp)

add_executable(parseTableGenerator source/parser/generator/generator.cpp
        source/parser/generator/grammar.cpp

        source/parser/ast_node.cpp
        source/parser/parser_action.cpp
        source/util/combine_hashes.cpp
)
target_link_libraries(parseTableGenerator lexer)

add_custom_command(OUTPUT parse_table.cpp
        COMMAND parseTableGenerator
        COMMENT "Generating `source/parser/parse_table.cpp`"
        DEPENDS parseTableGenerator
)

add_library(parser source/parser/parser.cpp
        source/parser/ast_node.cpp
        source/parser/parser_action.cpp
        source/parser/production_item.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/parse_table.cpp
        source/util/combine_hashes.cpp
)
add_library(type_checker source/type_checker/type_checker.cpp source/type_checker/type.cpp source/lexer/lexer.cpp source/util/commander_exception.cpp)
add_library(flow_controller source/flow_controller/flow_controller.cpp)
add_library(symbol_table source/symbol_table/symbol_table_organizer.cpp source/symbol_table/scope.cpp)
add_library(job_runner source/job_runner/job_runner.cpp)

add_executable(commander source/interpreter/commander.cpp)

target_link_libraries(commander lexer parser type_checker flow_controller symbol_table job_runner)

#  Test configurations:
#  --------------------

add_executable(lexerTests tests/lexer_tests.cpp)
target_link_libraries(lexerTests lexer GTest::gtest_main)
gtest_discover_tests(lexerTests)

add_executable(parserTests tests/parser_tests.cpp)
target_link_libraries(parserTests lexer parser type_checker GTest::gtest_main)
gtest_discover_tests(parserTests)

add_executable(typeCheckerTests tests/type_checker_tests.cpp)
target_link_libraries(typeCheckerTests lexer type_checker GTest::gtest_main)
gtest_discover_tests(typeCheckerTests)

add_executable(symbolTableTests tests/symbol_table_tests.cpp)
target_link_libraries(symbolTableTests symbol_table GTest::gtest_main)
gtest_discover_tests(symbolTableTests)

add_executable(flowControllerTests tests/flow_controller_tests.cpp)
target_link_libraries(flowControllerTests lexer parser type_checker flow_controller symbol_table GTest::gtest_main)
gtest_discover_tests(flowControllerTests)

FILE(GLOB builtins source/job_runner/builtins/*.cpp)
add_executable(jobRunnerTests ${builtins} tests/job_runner_tests.cpp)
target_link_libraries(jobRunnerTests lexer job_runner GTest::gtest_main)
gtest_discover_tests(jobRunnerTests)
