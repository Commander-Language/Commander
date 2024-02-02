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
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
