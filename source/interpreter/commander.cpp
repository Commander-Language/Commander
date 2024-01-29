#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../util/commander_exception.hpp"
#include "../type_checker/type_checker.hpp"
#include "../flow_controller/flow_controller.hpp"
#include <fstream>
#include <iostream>

void interpretFile(std::string& fileName, std::string& option, Parser::Parser& parser, TypeChecker::TypeChecker& typeChecker) {
    lexer::TokenList tokens;
    lexer::tokenize(tokens, fileName);
    if (option == "-l") {
        for (const lexer::TokenPtr& token : tokens) std::cout << token->toString() << '\n';
        return;
    }
    Parser::ASTNodeList nodes = parser.parse(tokens);
    if (option == "-p") {
        for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << '\n';
        return;
    }
    if (option == "-t") {
        typeChecker.typeCheck(nodes);
        for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << '\n';
        return;
    }
    FlowController::FlowController flow(nodes);
    flow.runtime();
}

int main(int argc, char** argv) {
    try {
        clock_t start = clock();
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker;
        clock_t end = clock();
        std::cout << "Parse Table Initialization Time: " << ((double)(end - start) / CLOCKS_PER_SEC) << " seconds"
                  << '\n';
        std::cout << "Commander Language Prototype" << '\n';
        std::cout << "Basic REPL for Commander scripting language." << '\n';
        std::string arg;
        if (argc > 1) { arg = std::string(argv[1]); }
        if (arg == "-f") {
            if (argc == 2) {
                throw util::CommanderException("No file name provided.");
            }
            std::string file = std::string(argv[2]);
            std::string option = "-l";
            interpretFile(file, option, parser, typeChecker);
            return 0;
        }
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

                interpretFile(tmpFileName, arg, parser, typeChecker);

                std::remove(tmpFileName.c_str());
            } catch (const util::CommanderException& err) { std::cout << err.what() << '\n'; }
        }
        return 0;
    } catch (const util::CommanderException& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
}