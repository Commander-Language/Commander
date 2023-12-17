/**
 * @file parser_tests.hpp
 * @brief Contains header content for parser_tests.cpp
 *
 */

#ifndef COMMANDER_PARSER_TESTS_HPP
#define COMMANDER_PARSER_TESTS_HPP

#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include <gtest/gtest.h>

/**
 * Parser used in the tests (initialized here to prevent initializing it for every test)
 */
Parser::Parser parser;

/**
 * @brief Represents the format for parser tests that successfully parse. Tests take in a file path to file to parse,
 * and another file path to file with expected S-Expression output.
 */
struct ParserParseTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

/**
 * @brief Represents the format for parser tests that should fail (i.e. throw an exception). Tests take in a file to
 * parse.
 */
struct ParserFailTests : ::testing::TestWithParam<std::string> {};

/**
 * Creates test suite, with list of all the files that need to be parsed with their expected S-Expression output
 */
INSTANTIATE_TEST_SUITE_P(
        ParserTestFiles, ParserParseTests,
        ::testing::Values(
                std::make_tuple("000.cmdr", "000.cmdr.expected"), std::make_tuple("001.cmdr", "001.cmdr.expected"),
                std::make_tuple("002.cmdr", "002.cmdr.expected"), std::make_tuple("003.cmdr", "003.cmdr.expected"),
                std::make_tuple("004.cmdr", "004.cmdr.expected"), std::make_tuple("005.cmdr", "005.cmdr.expected"),
                std::make_tuple("006.cmdr", "006.cmdr.expected"), std::make_tuple("007.cmdr", "007.cmdr.expected"),
                std::make_tuple("008.cmdr", "008.cmdr.expected"), std::make_tuple("009.cmdr", "009.cmdr.expected"),
                std::make_tuple("010.cmdr", "010.cmdr.expected"), std::make_tuple("011.cmdr", "011.cmdr.expected"),
                std::make_tuple("012.cmdr", "012.cmdr.expected"), std::make_tuple("013.cmdr", "013.cmdr.expected")));

/**
 * Creates test suite, with list of all the files that need to be parsed that are expected to fail (as a result of a
 * CommanderException being thrown)
 */
INSTANTIATE_TEST_SUITE_P(ParserTestFiles, ParserFailTests, ::testing::Values("000.cmdr", "001.cmdr"));

/**
 * @brief Takes in an ASTNodeList, and ensures they match what is contained in the expected output string
 *
 * @param nodes The ASTNodeList
 * @param expectedOutput The string containing the expected S-Expression output
 */
void expectOutputEqualsSExpressions(const Parser::ASTNodeList& nodes, const std::string& expectedOutput);

#endif  // COMMANDER_PARSER_TESTS_HPP