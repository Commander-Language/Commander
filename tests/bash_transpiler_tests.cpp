/**
 * @file bash_transpiler_tests.cpp
 * @brief Tests for the bash transpiler
 */

#include "flow_controller_tests.hpp"
#include "source/bash_transpiler/transpiler.hpp"
#include "source/util/commander_exception.hpp"
#include <cstdlib>

/**
 * Copied directly from
 * https://stackoverflow.com/questions/32039852/returning-output-from-bash-script-to-calling-c-function Big thanks to
 * user MeetTitan who originally wrote it!
 *
 * The helper essentially takes in a command as a string and returns the output as a string.
 * @param cmd The command to run
 * @return The string output
 */
std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) result += buffer;
    }
    pclose(pipe);
    return result;
}

void transpileAndRunFile(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    Parser::ASTNodeList nodes = parser.parse(tokens);
    TypeChecker::TypeChecker typeChecker(parser);
    TypeChecker::VariableTablePtr table = typeChecker.typeCheck(nodes);
    BashTranspiler::BashTranspiler transpiler(table);
    std::string const bashScript = transpiler.transpile(nodes);
    std::string const tmpFileName = std::tmpnam(nullptr);
    std::ofstream tmp(tmpFileName);
    tmp << bashScript;
    tmp.close();
    std::string const command = "bash " + tmpFileName + " 2>&1";
    exec(command.c_str());
    std::remove(tmpFileName.c_str());
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
    TypeChecker::VariableTablePtr table;
    try {
        table = typeChecker.typeCheck(nodes);
    } catch (const Util::CommanderException& e) {
        std::cout << "Type Checker Error: " << e.what() << "\n";
        FAIL();
    }

    // Transpile
    BashTranspiler::BashTranspiler transpiler(table);
    std::string bashScript = transpiler.transpile(nodes);
    std::string tmpFileName = std::tmpnam(nullptr);
    std::ofstream tmp(tmpFileName);
    tmp << bashScript;
    tmp.close();

    // Run
    std::string command = "bash " + tmpFileName + " 2>&1";

    std::string output = exec(command.c_str());
    if (output == "ERROR") {
        std::remove(tmpFileName.c_str());
        std::cout << "Bash Transpiler Error: Something went wrong running the script using popen()";
        FAIL();
    }

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
