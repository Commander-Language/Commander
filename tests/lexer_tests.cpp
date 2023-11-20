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
void expectOutputEqualsTokens(const lexer::TokenList& tokens, const std::string& expectedOutput) {
    std::ostringstream builder;
    for (const lexer::TokenPtr& token : tokens) builder << token->toString() << "\n";
    EXPECT_EQ(expectedOutput, builder.str());
}

/**
 * Runs the lexer tests from the tests/files/lexer_tests/should_lex directory
 */
TEST_P(LexerLexTests, ShouldLexFileAndMatchExpectedExamples) {
    auto params = GetParam();

    const std::string filePath = "tests/files/lexer_tests/should_lex/" + std::get<0>(params);
    const std::string expectedFilePath = "tests/files/lexer_tests/should_lex/" + std::get<1>(params);

    try {
        lexer::TokenList tokens;
        lexer::tokenize(tokens, filePath);
        const std::string expectedOutput = lexer::readFile(expectedFilePath);

        expectOutputEqualsTokens(tokens, expectedOutput);
    } catch (const util::CommanderException& e) {
        std::cout << "Lexer Error: " << e.what() << "\n";
        FAIL();
    }
}

/**
 * Runs the lexer tests from the tests/files/lexer_tests/should_fail directory
 */
TEST_P(LexerFailTests, ShouldNotLexFile) {
    auto param = GetParam();
    const std::string filePath = "tests/files/lexer_tests/should_fail/" + param;
    lexer::TokenList tokens;
    ASSERT_THROW(lexer::tokenize(tokens, filePath), util::CommanderException);
}

/**
 * Tests that passing in a non-existent file path results in an exception being thrown
 */
TEST(LexerFailTests, ShouldFailIfFileNotFound) {
    lexer::TokenList tokens;
    ASSERT_THROW(lexer::tokenize(tokens, "non-existent-file-name.txt"), util::CommanderException);
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