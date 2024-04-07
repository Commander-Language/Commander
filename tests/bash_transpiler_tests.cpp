/**
 * @file bash_transpiler_tests.cpp
 * @brief Tests for the bash transpiler
 */

#include "flow_controller_tests.hpp"
#include "source/bash_transpiler/transpiler.hpp"
#include "source/util/commander_exception.hpp"
#include <cstdlib>

void transpileAndRunFile(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    Parser::ASTNodeList nodes = parser.parse(tokens);
    TypeChecker::TypeChecker typeChecker(parser);
    typeChecker.typeCheck(nodes);
    BashTranspiler::BashTranspiler transpiler;
    std::string const bashScript = transpiler.transpile(nodes);
    std::string const tmpFileName = std::tmpnam(nullptr);
    std::ofstream tmp(tmpFileName);
    tmp << bashScript;
    tmp.close();
    std::string const command = "bash " + tmpFileName;
    int returnCode = system(command.c_str());
    std::remove(tmpFileName.c_str());
    if (returnCode != 0) { throw Util::CommanderException("Bash Transpiler Error: Got status code " + returnCode); }
}

/**
 * Runs the bash transpiler tests from the tests/files/flow_controller_tests/should_run directory
 */
TEST_P(FlowControllerPassTests, ShouldRunFileAndMatchExpectedExamplesBash) {
    auto params = GetParam();

    const std::string filePath = "../tests/files/flow_controller_tests/should_run/" + std::get<0>(params);
    const std::string expectedFilePath = "../tests/files/bash_transpiler_tests/should_run/" + std::get<1>(params);

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

    // Transpile
    BashTranspiler::BashTranspiler transpiler;
    std::string bashScript = transpiler.transpile(nodes);
    std::string tmpFileName = std::tmpnam(nullptr);
    std::ofstream tmp(tmpFileName);
    tmp << bashScript;
    tmp.close();

    // Set up std out to a stream
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Run
    std::string command = "bash " + tmpFileName;
    int returnCode = system(command.c_str());
    if (returnCode != 0) {
        std::remove(tmpFileName.c_str());
        std::cout.rdbuf(old);
        std::cout << "Bash Transpiler Error: Got status code " << returnCode << "\n";
        FAIL();
    }

    // Get output
    std::cout.rdbuf(old);
    std::string output = buffer.str();

    // Delete file
    std::remove(tmpFileName.c_str());

    // Check that output is equal to expected
    EXPECT_EQ(Lexer::readFile(expectedFilePath), output);
}

/**
 * Runs the bash transpiler tests from the tests/files/flow_controller_tests/should_fail directory
 */
TEST_P(FlowControllerFailTests, ShouldFailRunBash) {
    auto param = GetParam();
    const std::string filePath = "../tests/files/flow_controller_tests/should_fail/" + param;
    ASSERT_THROW(transpileAndRunFile(filePath), Util::CommanderException);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
