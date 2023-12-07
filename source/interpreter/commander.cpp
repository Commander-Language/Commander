#include <iostream>
#include <fstream>
#include "../lexer/lexer.hpp"
#include "../util/commander_exception.hpp"

int main(int argc, char** argv) {
    try {
        if (argc == 1) {
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

                for (const lexer::TokenPtr& token : tokens) std::cout << token->toString() << "\n";

                std::remove(tmpFileName.c_str());
            }
        }
        std::string fileName = std::string(argv[argc - 1]);
        if (fileName.rfind("-", 0) == 0) {
            throw util::CommanderException("No file provided", {});
        }
        std::string arg = std::string(argv[1]);
        //Lex file
        lexer::TokenList tokens;
        lexer::tokenize(tokens, fileName);
        if (arg == "-l") {
            for (const lexer::TokenPtr& token : tokens) std::cout << token->toString() << std::endl;
            return 0;
        }
        if (arg == "-p") {
            //TODO: parsing
        }
        if (arg == "-t") {
            //TODO: type checking
        }
        //TODO: Run script
        return 0;
    } catch (const util::CommanderException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}