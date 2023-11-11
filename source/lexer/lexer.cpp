/**
 * @file lexer.cpp
 * @brief Contains implementation for functions in lexer.hpp
 *
 */

#include "lexer.hpp"

namespace Lexer {

    std::string tokenTypeToString(const TokenType& type) {
        // TODO: Implement
        throw new std::exception();
    }

    std::string readFile(const std::string& filePath) {
        // TODO: Implement
        throw new std::exception();
    }

    void tokenize(const TokenList& tokens, const std::string& filePath) {
        // TODO: Implement
        throw new std::exception();
    }

    FilePosition skipWhitespace(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexOp(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexTokenLiteral(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexFloat(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexInt(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexString(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexVariable(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexCommandVariable(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }


    Token lexCommandString(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    bool isLetter(const char& character) {
        // TODO: Implement
        throw new std::exception();
    }

    bool isDigit(const char& character) {
        // TODO: Implement
        throw new std::exception();
    }

    bool isWhitespace(const char& character) {
        // TODO: Implement
        throw new std::exception();
    }

    bool isIllegalCharacter(const char& character) {
        // TODO: Implement
        throw new std::exception();
    }

}  // namespace Lexer