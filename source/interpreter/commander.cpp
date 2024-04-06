/**
 * @file commander.cpp
 * @brief This file contains the main program entry point for the Commander interpreter.
 */

#include "repl.hpp"
#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/type_checker/type_checker.hpp"

#include <fstream>
#include <iostream>

/**
 * @brief Reports whether the given string argument is present within the list of command-line arguments.
 *
 * @param arguments The command-line arguments from the user.
 * @param argument The argument to search for.
 * @return True if the given argument is in the given list of arguments.
 */
bool hasArgument(std::vector<std::string>& arguments, const std::string& argument) {
    return std::find(arguments.begin(), arguments.end(), argument) != arguments.end();
}

/**
 * @brief Reports the value of the given argument from a command-line invocation.
 * @details For example, in `program --foo bar`, the value of argument `--foo` is `bar`.
 *
 * @param arguments The command-line arguments from the user.
 * @param argument The argument to find and report its value.
 * @return The value of the given argument.
 */
std::string getArgumentValue(std::vector<std::string>& arguments, const std::string& argument) {
    auto iter = std::find(arguments.begin(), arguments.end(), argument);
    if (iter == arguments.end()) return {};
    ++iter;
    if (iter == arguments.end()) return {};
    return *iter;
}

/**
 * @brief Interprets the given file using the given parser, type-checker, and flow controller.
 *
 * @param fileName The name of the file to interpret.
 * @param arguments The command-line arguments from the user.
 * @param parser The Commander parser.
 * @param typeChecker The Commander type-checker.
 * @param controller The Commander flow controller (logic interpreter).
 */
void interpretFile(const std::string& fileName, std::vector<std::string>& arguments, Parser::Parser& parser,
                   TypeChecker::TypeChecker& typeChecker, FlowController::FlowController& controller) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, fileName);

    // Lex only:
    if (hasArgument(arguments, "-l")) {
        for (const Lexer::TokenPtr& token : tokens) Util::println(token->toString());
        return;
    }

    // Lex and parse:
    Parser::ASTNodeList nodes = parser.parse(tokens);
    if (hasArgument(arguments, "-p")) {
        for (const Parser::ASTNodePtr& node : nodes) Util::println(node->sExpression());
        return;
    }

    // Lex, parse, and type-check:
    typeChecker.typeCheck(nodes);
    if (hasArgument(arguments, "-t")) {
        for (const Parser::ASTNodePtr& node : nodes) Util::println(node->sExpression());
        return;
    }

    // Lex, parse, type-check, and transpile to Bash:
    if (hasArgument(arguments, "-b")) {
        std::string outFile = getArgumentValue(arguments, "-o");
        if (outFile.empty()) { outFile = "bash-out.sh"; }
        // TODO: Implement bash transpiler
        /*BashTranspiler::BashTranspiler transpiler;
        std::string bashOutput = transpiler.transpile(nodes);
        if (Util::usingNCurses) {
            Util::println(bashOutput);
        } else {
            Util::writeToFile(bashOutput, outFile);
        }*/
        return;
    }

    // Interpret the file:
    controller.runtime(nodes);
}

/**
 * @brief Main program entry point.
 *
 * @param argc The number of provided command-line arguments.
 * @param argv The command-line arguments from the user.
 * @return 0 on success; 1 on error.
 */
int main(const int argc, const char** argv) {
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++) arguments.emplace_back(argv[i]);

    try {
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker(parser);
        FlowController::FlowController controller;

        // If the user provided a file with the `-f` argument, interpret that file:
        if (hasArgument(arguments, "-f")) {
            const std::string file = getArgumentValue(arguments, "-f");
            if (file.empty()) { throw Util::CommanderException("No file provided for -f flag"); }
            interpretFile(file, arguments, parser, typeChecker, controller);
            return 0;
        }

        // Otherwise, run the REPL:
        const auto runProgram = [&](const std::string& programText) {
            // Make a temporary file for the lexer.
            const std::string tmpFileName = std::tmpnam(nullptr);  // NOLINT(*-mt-unsafe)
            std::ofstream tmp(tmpFileName);
            tmp << programText;
            tmp.close();  // Close file here to save changes

            interpretFile(tmpFileName, arguments, parser, typeChecker, controller);

            if (std::remove(tmpFileName.c_str()) != 0) {  // NOLINT(*-mt-unsafe)
                throw Util::CommanderException("I/O error: could not delete temporary file");
            }
        };
        REPL {runProgram}.run();

        return 0;
    } catch (const Util::CommanderException& e) {
        // Report the error and exit.
        std::cout << e.what() << '\n';
        return 1;
    }
}
