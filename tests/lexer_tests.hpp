/**
 * @file lexer_tests.hpp
 * @brief Contains header content for lexer_tests.cpp
 *
 */

#ifndef COMMANDER_LEXER_TESTS_HPP
#define COMMANDER_LEXER_TESTS_HPP

#include "../source/lexer/lexer.hpp"
#include "../source/util/commander_exception.hpp"
#include <gtest/gtest.h>

/**
 * @brief Represents the format for lexer tests. Tests take in a file path to file to lex, and another file path to file
 * with expected token output.
 */
struct LexerTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

INSTANTIATE_TEST_SUITE_P(
        LexerTestFiles, LexerTests,
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
                std::make_tuple("026.cmdr", "026.cmdr.expected"), std::make_tuple("027.cmdr", "027.cmdr.expected"),
                std::make_tuple("028.cmdr", "028.cmdr.expected"), std::make_tuple("029.cmdr", "029.cmdr.expected"),
                std::make_tuple("030.cmdr", "030.cmdr.expected"), std::make_tuple("031.cmdr", "031.cmdr.expected"),
                std::make_tuple("032.cmdr", "032.cmdr.expected"), std::make_tuple("033.cmdr", "033.cmdr.expected"),
                std::make_tuple("034.cmdr", "034.cmdr.expected"), std::make_tuple("035.cmdr", "035.cmdr.expected"),
                std::make_tuple("036.cmdr", "036.cmdr.expected"), std::make_tuple("037.cmdr", "037.cmdr.expected"),
                std::make_tuple("038.cmdr", "038.cmdr.expected"), std::make_tuple("039.cmdr", "039.cmdr.expected"),
                std::make_tuple("040.cmdr", "040.cmdr.expected"), std::make_tuple("041.cmdr", "041.cmdr.expected"),
                std::make_tuple("042.cmdr", "042.cmdr.expected"), std::make_tuple("043.cmdr", "043.cmdr.expected"),
                std::make_tuple("044.cmdr", "044.cmdr.expected"), std::make_tuple("045.cmdr", "045.cmdr.expected"),
                std::make_tuple("046.cmdr", "046.cmdr.expected"), std::make_tuple("047.cmdr", "047.cmdr.expected"),
                std::make_tuple("048.cmdr", "048.cmdr.expected"), std::make_tuple("049.cmdr", "049.cmdr.expected"),
                std::make_tuple("050.cmdr", "050.cmdr.expected"), std::make_tuple("051.cmdr", "051.cmdr.expected"),
                std::make_tuple("052.cmdr", "052.cmdr.expected"), std::make_tuple("053.cmdr", "053.cmdr.expected"),
                std::make_tuple("054.cmdr", "054.cmdr.expected"), std::make_tuple("055.cmdr", "055.cmdr.expected"),
                std::make_tuple("056.cmdr", "056.cmdr.expected"), std::make_tuple("057.cmdr", "057.cmdr.expected"),
                std::make_tuple("058.cmdr", "058.cmdr.expected"), std::make_tuple("059.cmdr", "059.cmdr.expected"),
                std::make_tuple("060.cmdr", "060.cmdr.expected"), std::make_tuple("061.cmdr", "061.cmdr.expected"),
                std::make_tuple("062.cmdr", "062.cmdr.expected")));

/**
 * @brief Takes in the list of tokens, and ensures they match what is contained in the expected output string
 *
 * @param tokens The list of tokens
 * @param expectedOutput The string containing the expected token output
 */
void expectOutputEqualsTokens(const lexer::TokenList& tokens, const std::string& expectedOutput);

#endif  // COMMANDER_LEXER_TESTS_HPP
