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
void expectOutputEqualsTokens(const Lexer::TokenList& tokens, const std::string& expectedOutput) {
    std::ostringstream builder;
    for (int i = 0; i < tokens.size() - 1; i++) { builder << tokens[i]->toString() << "\n"; }
    EXPECT_EQ(expectedOutput, builder.str());
}

/**
 * Runs the lexer tests from the tests/files/lexer_tests/should_lex directory
 */
TEST_P(LexerLexTests, ShouldLexFileAndMatchExpectedExamples) {
    auto params = GetParam();

    const std::string filePath = "../tests/files/lexer_tests/should_lex/" + std::get<0>(params);
    const std::string expectedFilePath = "../tests/files/lexer_tests/should_lex/" + std::get<1>(params);

    try {
        Lexer::TokenList tokens;
        Lexer::tokenize(tokens, filePath);
        const std::string expectedOutput = Lexer::readFile(expectedFilePath);

        expectOutputEqualsTokens(tokens, expectedOutput);
    } catch (const Util::CommanderException& e) {
        std::cout << "Lexer Error: " << e.what() << "\n";
        FAIL();
    }
}

/**
 * Runs the lexer tests from the tests/files/lexer_tests/should_fail directory
 */
TEST_P(LexerFailTests, ShouldNotLexFile) {
    auto param = GetParam();
    const std::string filePath = "../tests/files/lexer_tests/should_fail/" + param;
    Lexer::TokenList tokens;
    ASSERT_THROW(Lexer::tokenize(tokens, filePath), Util::CommanderException);
}

/**
 * Tests that passing in a non-existent file path results in an exception being thrown
 */
TEST(LexerFailTests, ShouldFailIfFileNotFound) {
    Lexer::TokenList tokens;
    ASSERT_THROW(Lexer::tokenize(tokens, "non-existent-file-name.txt"), Util::CommanderException);
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