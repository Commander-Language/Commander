#include "repl.hpp"
#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/type_checker/type_checker.hpp"

#include <fstream>
#include <iostream>

bool hasArgument(std::vector<std::string>& arguments, const std::string& argument) {
    return std::find(arguments.begin(), arguments.end(), argument) != arguments.end();
}

std::string getArgumentValue(std::vector<std::string>& arguments, const std::string& argument) {
    auto iter = std::find(arguments.begin(), arguments.end(), argument);
    if (iter == arguments.end()) return {};
    ++iter;
    if (iter == arguments.end()) return {};
    return *iter;
}

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

int main(const int argc, const char** argv) {
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++) arguments.emplace_back(argv[i]);

    try {
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker(parser);
        FlowController::FlowController controller;

        // Interpret file:
        if (hasArgument(arguments, "-f")) {
            const std::string file = getArgumentValue(arguments, "-f");
            if (file.empty()) { throw Util::CommanderException("No file provided for -f flag"); }
            interpretFile(file, arguments, parser, typeChecker, controller);
            return 0;
        }

        // Otherwise, run the REPL:
        REPL {[&](const std::string& fileName) {
            interpretFile(fileName, arguments, parser, typeChecker, controller);
        }}.run();

        return 0;
    } catch (const Util::CommanderException& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
}
