#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type_checker.hpp"
#include "source/util/commander_exception.hpp"
#include <fstream>
#include <iostream>

void interpretFile(std::string& fileName, std::vector<std::string>& arguments, Parser::Parser& parser,
                   TypeChecker::TypeChecker& typeChecker) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, fileName);
    if (std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        for (const Lexer::TokenPtr& token : tokens) std::cout << token->toString() << '\n';
        return;
    }
    Parser::ASTNodeList nodes = parser.parse(tokens);
    if (std::find(arguments.begin(), arguments.end(), "-p") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << '\n';
        return;
    }
    typeChecker.typeCheck(nodes);
    if (std::find(arguments.begin(), arguments.end(), "-t") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << '\n';
        return;
    }
    FlowController::FlowController controller(nodes);
    controller.runtime();
}

int main(int argc, char** argv) {
    try {
        std::vector<std::string> arguments;
        for (int i = 1; i < argc; i++) { arguments.push_back(argv[i]); }
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker;
        auto fileIterator = std::find(arguments.begin(), arguments.end(), "-f");
        if (fileIterator++ != arguments.end()) {
            if (fileIterator == arguments.end()) { throw Util::CommanderException("No file name provided."); }
            std::string file = *fileIterator;
            interpretFile(file, arguments, parser, typeChecker);
            return 0;
        }
        std::cout << "Commander Language Version Beta" << '\n';
        std::cout << "Basic REPL for Commander scripting language." << '\n';
        while (true) {
            try {
                std::cout << ">> ";

                std::string source;
                std::getline(std::cin, source);

                if (source == "clear") {
                    std::cout << "\033[2J\033[H" << '\n';
                    continue;
                }

                if (source == "exit") { return 0; }

                // Make a temporary file for the lexer
                std::string tmpFileName = std::tmpnam(nullptr);  // Not thread safe!!
                std::ofstream tmp(tmpFileName);
                tmp << source;
                tmp.close();  // close file here to save changes

                interpretFile(tmpFileName, arguments, parser, typeChecker);

                std::remove(tmpFileName.c_str());
            } catch (const Util::CommanderException& err) { std::cout << err.what() << '\n'; }
        }
        return 0;
    } catch (const Util::CommanderException& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
}