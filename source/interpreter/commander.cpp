#include <iostream>
#include <fstream>
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../util/commander_exception.hpp"

int main(int argc, char** argv) {
    try {
        std::string arg = "";
        if (argc > 1) { arg = std::string(argv[1]); }
        Parser::Parser parser;
        std::cout << "Commander Scripting Language version x.x\n";
        std::cout << "Basic REPL for Commander scripting language. Currently just prints out token "
                     "representation of input.\n";
        while (true) {
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
                //TODO: type checking
                continue;
            }
            //TODO: Run script

        }
        return 0;
    } catch (const util::CommanderException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}