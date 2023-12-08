#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
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
        clock_t end = clock();
        std::cout << "Parse Initialization Time: " << ((double)(end - start) / CLOCKS_PER_SEC) << " seconds"
                  << std::endl;
        std::cout << "Commander Scripting Language version x.x\n";
        std::cout << "Basic REPL for Commander scripting language."
                     "representation of input.\n";
        while (true) {
            try {
                std::cout << ">> ";

                std::string source;
                std::getline(std::cin, source);

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
                    // TODO: type checking
                    continue;
                }
                // TODO: Run script
                std::map<std::string, std::variant<long, double, bool>> variables;
                for (const Parser::ASTNodePtr& node : nodes) {
                    if (node->nodeType() == Parser::ASTNodeType::STMTS) {
                        switch (node->nodeType()) {
                            // Basic commands
                            case Parser::ASTNodeType::CMD:
                                // TODO
                                break;
                            // Variables
                            case Parser::ASTNodeType::VARIABLE:
                                // TODO
                                break;
                            // Expressions (math, boolean, variables, strings and string interpolation)
                            case Parser::ASTNodeType::EXPRS:
                                // TODO
                                break;
                            // User I/O (need to handle read, write, scan, print)
                            case Parser::ASTNodeType::STMT:
                                // TODO
                                break;
                        }
                    }
                }
            } catch (const util::CommanderException& err) { std::cout << err.what() << std::endl; }
        }
        return 0;
    } catch (const util::CommanderException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}