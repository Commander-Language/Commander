/**
 * @file type_checker_tests.hpp
 * @brief Contains header content for type_checker_tests.cpp
 *
 */

#ifndef COMMANDER_TYPE_CHECKER_TESTS_HPP
#define COMMANDER_TYPE_CHECKER_TESTS_HPP

#include "source/parser/parser.hpp"
#include "source/type_checker/type_checker.hpp"
#include <gtest/gtest.h>

/**
 * Method that simplifies creating a shared pointer type
 * @tparam TType The type
 * @return The shared pointer type
 */
template<typename TType>
std::shared_ptr<TType> makeType() {
    return std::make_shared<TType>();
}

/**
 * Parser used in the tests (initialized here to prevent initializing it for every test)
 */
Parser::Parser parser;

/**
 * Type checker used in the tests (initialized here to prevent initializing it for every test)
 */
TypeChecker::TypeChecker typeChecker;

/**
 * @brief Represents the format for type checker tests that successfully type check. Tests take in a file path to file
 * to type checked, and another file path to file with expected S-Expression output.
 */
struct TypeCheckerPassTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

/**
 * @brief Represents the format for type checker tests that should fail (i.e. throw an exception). Tests take in a file
 * to type check.
 */
struct TypeCheckerFailTests : ::testing::TestWithParam<std::string> {};

/**
 * Creates test suite, with list of all the files that need to be type checked with their expected S-Expression output
 */
INSTANTIATE_TEST_SUITE_P(TypeCheckerTestFiles, TypeCheckerPassTests,
                         ::testing::Values(std::make_tuple("000.cmdr", "000.cmdr.expected"),
                                           std::make_tuple("001.cmdr", "001.cmdr.expected"),
                                           std::make_tuple("002.cmdr", "002.cmdr.expected"),
                                           std::make_tuple("003.cmdr", "003.cmdr.expected")));

/**
 * Creates test suite, with list of all the files that need to be type checked that are expected to fail (as a result of
 * a CommanderException being thrown)
 */
INSTANTIATE_TEST_SUITE_P(TypeCheckerTestFiles, TypeCheckerFailTests,
                         ::testing::Values("000.cmdr", "001.cmdr", "002.cmdr", "003.cmdr", "004.cmdr", "005.cmdr",
                                           "006.cmdr", "007.cmdr", "008.cmdr", "009.cmdr", "010.cmdr", "011.cmdr",
                                           "012.cmdr", "013.cmdr", "014.cmdr", "015.cmdr", "016.cmdr", "017.cmdr",
                                           "018.cmdr", "019.cmdr", "020.cmdr", "021.cmdr", "022.cmdr", "023.cmdr",
                                           "024.cmdr", "025.cmdr", "026.cmdr", "027.cmdr", "028.cmdr", "029.cmdr",
                                           "030.cmdr", "031.cmdr", "032.cmdr", "033.cmdr", "034.cmdr", "035.cmdr",
                                           "036.cmdr", "037.cmdr", "038.cmdr", "039.cmdr", "040.cmdr", "041.cmdr",
                                           "042.cmdr", "043.cmdr", "044.cmdr", "045.cmdr", "046.cmdr", "047.cmdr",
                                           "048.cmdr", "049.cmdr", "050.cmdr", "051.cmdr", "052.cmdr"));

/**
 * @brief Takes in a file path, and goes through lexing, parsing, and type checking it
 * @param filePath The file path
 */
void lexParseAndTypeCheck(const std::string& filePath);

/**
 * @brief Takes in an ASTNodeList, and ensures they match what is contained in the expected output string
 *
 * @param nodes The ASTNodeList
 * @param expectedOutput The string containing the expected S-Expression output
 */
void expectOutputEqualsSExpressions(const Parser::ASTNodeList& nodes, const std::string& expectedOutput);

#endif  // COMMANDER_TYPE_CHECKER_TESTS_HPP