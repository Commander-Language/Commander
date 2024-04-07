/**
 * @file parser_tests.cpp
 * @brief Contains tests for the parser
 *
 */

#include "parser_tests.hpp"
#include "source/lexer/lexer.hpp"
#include "source/util/commander_exception.hpp"

void expectOutputEqualsSExpressions(const Parser::ASTNodeList& nodes, const std::string& expectedOutput) {
    std::ostringstream builder;
    for (const Parser::ASTNodePtr& node : nodes) builder << node->sExpression() << "\n";
    EXPECT_EQ(expectedOutput, builder.str());
}

void lexAndParse(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    parser.parse(tokens);
}

/**
 * Runs the parser tests from the tests/files/parser_tests/should_parse directory
 */
TEST_P(ParserParseTests, ShouldParseFileAndMatchExpectedExamples) {
    auto params = GetParam();

    const std::string filePath = "../tests/files/parser_tests/should_parse/" + std::get<0>(params);
    const std::string expectedFilePath = "../tests/files/parser_tests/should_parse/" + std::get<1>(params);

    // Lex
    Lexer::TokenList tokens;
    try {
        Lexer::tokenize(tokens, filePath);
    } catch (const Util::CommanderException& e) {
        std::cout << "Lexer Error: " << e.what() << "\n";
        FAIL();
    }

    // Parse
    try {
        Parser::ASTNodeList const nodes = parser.parse(tokens);
        const std::string expectedOutput = Lexer::readFile(expectedFilePath);
        expectOutputEqualsSExpressions(nodes, expectedOutput);
    } catch (const Util::CommanderException& e) {
        std::cout << "Parser Error: " << e.what() << "\n";
        FAIL();
    }
}

/**
 * Runs the parser tests from the tests/files/parser_tests/should_fail directory
 */
TEST_P(ParserFailTests, ShouldNotParseFile) {
    auto param = GetParam();
    const std::string filePath = "../tests/files/parser_tests/should_fail/" + param;
    ASSERT_THROW(
            {
                try {
                    lexAndParse(filePath);
                } catch (const std::out_of_range& e) { throw Util::CommanderException(e.what()); }
            },
            Util::CommanderException);
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