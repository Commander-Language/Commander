/**
 * @file flow_controller_tests.hpp
 * @brief Contains header content for flow_controller_tests.cpp
 *
 */

#ifndef COMMANDER_FLOW_CONTROLLER_TESTS_HPP
#define COMMANDER_FLOW_CONTROLLER_TESTS_HPP

#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type_checker.hpp"
#include "source/util/commander_exception.hpp"
#include <gtest/gtest.h>
#include <fstream>

/**
 * Parser used in the tests (initialized here to prevent initializing it for every test)
 */
Parser::Parser parser;

/**
 * Type checker used in the tests (initialized here to prevent initializing it for every test)
 */
TypeChecker::TypeChecker typeChecker;

/**
 * @brief Represents the format for flow controller tests that successfully run. Tests take in a file path to file to
 * run, and another file path to file with expected output.
 */
struct FlowControllerPassTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

/**
 * @brief Represents the format for flow controller tests that should fail (i.e. throw an exception). Tests take in a
 * file to run.
 */
struct FlowControllerFailTests : ::testing::TestWithParam<std::string> {};

/**
 * Creates test suite, with list of all the files that need to be ran with their expected output
 */
INSTANTIATE_TEST_SUITE_P(
        FlowControllerTestFiles, FlowControllerPassTests,
        ::testing::Values(
                std::make_tuple("000.cmdr", "000.cmdr.expected"), std::make_tuple("001.cmdr", "001.cmdr.expected"),
                std::make_tuple("002.cmdr", "002.cmdr.expected"), std::make_tuple("003.cmdr", "003.cmdr.expected"),
                std::make_tuple("004.cmdr", "004.cmdr.expected"), std::make_tuple("005.cmdr", "005.cmdr.expected"),
                std::make_tuple("006.cmdr", "006.cmdr.expected"), std::make_tuple("007.cmdr", "007.cmdr.expected"),
                std::make_tuple("008.cmdr", "008.cmdr.expected"), std::make_tuple("009.cmdr", "009.cmdr.expected"),
                std::make_tuple("010.cmdr", "010.cmdr.expected"), std::make_tuple("011.cmdr", "011.cmdr.expected"),
                std::make_tuple("012.cmdr", "012.cmdr.expected"), std::make_tuple("013.cmdr", "013.cmdr.expected"),
                std::make_tuple("014.cmdr", "014.cmdr.expected"), std::make_tuple("015.cmdr", "015.cmdr.expected"),
                std::make_tuple("016.cmdr", "016.cmdr.expected"), std::make_tuple("017.cmdr", "017.cmdr.expected"),
                std::make_tuple("018.cmdr", "018.cmdr.expected"), std::make_tuple("019.cmdr", "019.cmdr.expected"),
                std::make_tuple("020.cmdr", "020.cmdr.expected"), std::make_tuple("021.cmdr", "021.cmdr.expected"),
                std::make_tuple("022.cmdr", "022.cmdr.expected"), std::make_tuple("023.cmdr", "023.cmdr.expected"),
                std::make_tuple("024.cmdr", "024.cmdr.expected"), std::make_tuple("025.cmdr", "025.cmdr.expected"),
                std::make_tuple("026.cmdr", "026.cmdr.expected")));

/**
 * Creates test suite, with list of all the files that need to be ran that are expected to fail (as a result of a
 * CommanderException being thrown)
 */
INSTANTIATE_TEST_SUITE_P(FlowControllerTestFiles, FlowControllerFailTests,
                         ::testing::Values("000.cmdr", "001.cmdr", "002.cmdr", "003.cmdr", "004.cmdr", "005.cmdr",
                                           "006.cmdr", "007.cmdr", "008.cmdr", "009.cmdr", "010.cmdr", "011.cmdr",
                                           "012.cmdr", "013.cmdr", "014.cmdr", "015.cmdr", "016.cmdr", "017.cmdr",
                                           "018.cmdr", "019.cmdr"));

#endif  // COMMANDER_FLOW_CONTROLLER_TESTS_HPP