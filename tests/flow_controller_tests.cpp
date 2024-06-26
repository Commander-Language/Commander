/**
 * @file flow_controller_tests.cpp
 * @brief Tests for the flow controller
 */

#include "flow_controller_tests.hpp"

void runFile(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    Parser::ASTNodeList nodes = parser.parse(tokens);
    TypeChecker::TypeChecker typeChecker(parser);
    typeChecker.typeCheck(nodes);
    FlowController::FlowController controller;
    controller.runtime(nodes);
}

// TODO: Add tests for builtin functions that can't be tested using files due to changing values (e.g. random(), time(),
// date(), etc.)

/**
 * Runs the flow controller tests from the tests/files/flow_controller_tests/should_run directory
 */
TEST_P(FlowControllerPassTests, ShouldRunFileAndMatchExpectedExamples) {
    auto params = GetParam();

    const std::string filePath = "../tests/files/flow_controller_tests/should_run/" + std::get<0>(params);
    const std::string expectedFilePath = "../tests/files/flow_controller_tests/should_run/" + std::get<1>(params);

    // Lex
    Lexer::TokenList tokens;
    try {
        Lexer::tokenize(tokens, filePath);
    } catch (const Util::CommanderException& e) {
        std::cout << "Lexer Error: " << e.what() << "\n";
        FAIL();
    }

    // Parse
    Parser::ASTNodeList nodes;
    try {
        nodes = parser.parse(tokens);
    } catch (const Util::CommanderException& e) {
        std::cout << "Parser Error: " << e.what() << "\n";
        FAIL();
    }

    // Type Check
    TypeChecker::TypeChecker typeChecker(parser);
    try {
        typeChecker.typeCheck(nodes);
    } catch (const Util::CommanderException& e) {
        std::cout << "Type Checker Error: " << e.what() << "\n";
        FAIL();
    }

    // Set up std out to a stream
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Run
    FlowController::FlowController controller;
    try {
        controller.runtime(nodes);
    } catch (std::exception e) {
        std::cout.rdbuf(old);
        std::cout << "Flow Controller Error: " << e.what() << "\n";
        FAIL();
    }

    // Get output
    std::cout.rdbuf(old);
    std::string output = buffer.str();

    // Check that output is equal to expected
    EXPECT_EQ(Util::readFile(expectedFilePath), output);
}

/**
 * Runs the flow controller tests from the tests/files/flow_controller_tests/should_fail directory
 */
TEST_P(FlowControllerFailTests, ShouldFailRun) {
    auto param = GetParam();
    const std::string filePath = "../tests/files/flow_controller_tests/should_fail/" + param;
    ASSERT_THROW(runFile(filePath), Util::CommanderException);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
