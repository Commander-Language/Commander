/**
 * @file flow_controller_tests.cpp
 * @brief Tests for the flow controller
 */

#include "flow_controller_tests.hpp"

void runFile(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    Parser::ASTNodeList nodes = parser.parse(tokens);
    typeChecker.typeCheck(nodes);
    FlowController::FlowController controller(nodes);
    controller.runtime();
}

/**
 * @brief Test that check if saving expressions to variables works
 */
TEST(FlowControllerTests, SaveIntToVariable) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, "../tests/files/flow_controller_tests/script001.cmdr");
    Parser::ASTNodeList nodes = parser.parse(tokens);
    FlowController::FlowController controller(nodes);
    controller.runtime();

    EXPECT_TRUE(controller.hasVariable("one"));
    EXPECT_TRUE(controller.hasVariable("two"));
    EXPECT_TRUE(controller.hasVariable("three"));
    EXPECT_TRUE(controller.hasVariable("nine"));
    EXPECT_TRUE(controller.hasVariable("twenty"));
    EXPECT_TRUE(controller.hasVariable("eight_one"));
    EXPECT_TRUE(controller.hasVariable("mod5_2"));

    EXPECT_EQ(controller.getVariableValue("one"), 1);
    EXPECT_EQ(controller.getVariableValue("two"), 2);
    EXPECT_EQ(controller.getVariableValue("three"), 3);
    EXPECT_EQ(controller.getVariableValue("nine"), 9);
    EXPECT_EQ(controller.getVariableValue("twenty"), 20);
    EXPECT_EQ(controller.getVariableValue("eight_one"), 81);
    EXPECT_EQ(controller.getVariableValue("mod5_2"), 1);
}

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
    FlowController::FlowController controller(nodes);
    controller.runtime();

    // Get output
    std::cout.rdbuf(old);
    std::string output = buffer.str();

    // Check that output is equal to expected
    EXPECT_EQ(Lexer::readFile(expectedFilePath), output);
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
