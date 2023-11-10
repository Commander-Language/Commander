/**
 * @file lexer_tests.hpp
 * @brief Contains header content for lexer_tests.cpp
 *
 */

#ifndef COMMANDER_LEXER_TESTS_HPP
#define COMMANDER_LEXER_TESTS_HPP

#include "../source/lexer/lexer.hpp"
#include <gtest/gtest.h>

/**
 * @brief Represents the format for lexer tests. Tests take in a file path to file to lex, and another file path to file
 * with expected token output.
 */
struct LexerTests : ::testing::TestWithParam<std::tuple<const std::string&, const std::string&>> {};

INSTANTIATE_TEST_SUITE_P(LexerTestFiles, LexerTests,
                         ::testing::Values(std::make_tuple("000.cmdr", "000.cmdr.output"),
                                           std::make_tuple("001.cmdr", "001.cmdr.output")));

/**
 * @brief Takes in the list of tokens, and ensures they match what is contained in the expected output string
 *
 * @param tokens The list of tokens
 * @param expectedOutput The string containing the expected token output
 */
void expectOutputEqualsTokens(const TokenList tokens, const std::string& expectedOutput);

#endif  // COMMANDER_LEXER_TESTS_HPP
