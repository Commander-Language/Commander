/**
 * @file lexer_tests.cpp
 * @brief Contains tests for the lexer
 *
 */

#include "lexer_tests.hpp"

/**
 * Asserts that the given token list matches the expected token list given in the expected output
 * @param tokens The list of tokens
 * @param expectedOutput The expected token output
 */
void expectOutputEqualsTokens(const TokenList tokens, const std::string& expectedOutput) {
    std::ostringstream builder;
    for (const Token token : tokens) { builder << token.toString() << "\n"; }
    builder << "Compilation succeeded: lexical analysis complete\n";
    EXPECT_EQ(expectedOutput, builder.str());
}

/**
 * Runs the lexer tests from the tests/files/lexer_tests directory
 */
TEST_P(LexerTests, ShouldLexFileAndMatchExpectedExamples) {
    auto params = GetParam();

    std::string filePath = "tests/files/lexer_tests/" + std::get<0>(params);
    std::string expectedFilePath = "tests/files/lexer_tests/" + std::get<1>(params);

    TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    std::string expectedOutput = readFile(expectedFilePath);
    expectOutputEqualsTokens(tokens, expectedOutput);
}

/**
 * Main function that runs all the tests
 * @param argc Number of arguments
 * @param argv The list of arguments
 * @return Exit code from running the tests
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}