/**
 * @file flow_controller_tests.cpp
 * @brief Tests for the flow controller
 */
#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/parser/parser.hpp"
#include "source/util/commander_exception.hpp"
#include <fstream>
#include <gtest/gtest.h>

// Global Parser
Parser::Parser parser;

std::string getFileContents(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) throw Util::CommanderException("Flow Controller Tests: Can't find file");
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
/**
 * @brief
 */
TEST(FlowControllerTests, SaveIntToVariable) {
    try {
        Lexer::TokenList tokens;
        Lexer::tokenize(tokens, "../tests/files/flow_controller_tests/script001.cmdr");
        Parser::ASTNodeList nodes = parser.parse(tokens);
        FlowController::FlowController controller(nodes);
        controller.runtime();

        EXPECT_TRUE(controller.hasVariable("x"));
        EXPECT_EQ(controller.getVariableValue("x"), 1);

        EXPECT_TRUE(controller.hasVariable("y"));
        EXPECT_EQ(controller.getVariableValue("y"), 1);

        EXPECT_TRUE(controller.hasVariable("z"));
        EXPECT_EQ(controller.getVariableValue("z"), 2);

    } catch (const Util::CommanderException& err) { std::cout << "Error caught: " << err.what() << std::endl; }
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
