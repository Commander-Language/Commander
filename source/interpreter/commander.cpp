#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../type_checker/type_checker.hpp"
#include "../util/commander_exception.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <variant>

int main(int argc, char** argv) {
    try {
        std::string arg = "";
        if (argc > 1) { arg = std::string(argv[1]); }
        clock_t start = clock();
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker;
        clock_t end = clock();
        std::cout << "Parse Table Initialization Time: " << ((double)(end - start) / CLOCKS_PER_SEC) << " seconds"
                  << std::endl;
        std::cout << "Commander Language Prototype" << std::endl;
        std::cout << "Basic REPL for Commander scripting language." << std::endl;
        while (true) {
            try {
                std::cout << ">> ";

                std::string source;
                std::getline(std::cin, source);

                if (source == "clear") {
                    std::cout << "\033[2J\033[H" << std::endl;
                    continue;
                }

                if (source == "exit") { return 0; }

                // Make a temporary file for the lexer
                std::string tmpFileName = std::tmpnam(nullptr);  // Not thread safe!!
                std::ofstream tmp(tmpFileName);
                tmp << source;
                tmp.close();  // close file here to save changes

                lexer::TokenList tokens;
                lexer::tokenize(tokens, tmpFileName);
                std::remove(tmpFileName.c_str());
                if (arg == "-l") {
                    for (const lexer::TokenPtr& token : tokens) std::cout << token->toString() << std::endl;
                    continue;
                }
                Parser::ASTNodeList nodes = parser.parse(tokens);
                if (arg == "-p") {
                    for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << std::endl;
                    continue;
                }
                if (arg == "-t") {
                    typeChecker.typeCheck(nodes);
                    for (const Parser::ASTNodePtr& node : nodes) std::cout << node->sExpression() << std::endl;
                    continue;
                }
                // TODO: Run flow controller
            } catch (const util::CommanderException& err) { std::cout << err.what() << std::endl; }
        }
        return 0;
    } catch (const util::CommanderException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}