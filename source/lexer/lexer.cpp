/**
 * @file lexer.cpp
 * @brief Contains implementation for functions in lexer.hpp
 *
 */

#include "lexer.hpp"
#include "../util/commander_exception.hpp"

namespace Lexer {

    std::string tokenTypeToString(const TokenType& type) {
        switch (type) {
            case ALIAS:
                return "ALIAS";
            case AMPERSAND:
                return "AMPERSAND";
            case BOOL:
                return "BOOL";
            case CMDSTRINGVAL:
                return "CMDSTRINGVAL";
            case CMDVARIABLE:
                return "CMDVARIABLE";
            case COLON:
                return "COLON";
            case COMMA:
                return "COMMA";
            case CONST:
                return "CONST";
            case DO:
                return "DO";
            case ELSE:
                return "ELSE";
            case END_OF_FILE:
                return "END_OF_FILE";
            case EQUALS:
                return "EQUALS";
            case FALSE:
                return "FALSE";
            case FLOAT:
                return "FLOAT";
            case FLOATVAL:
                return "FLOATVAL";
            case FOR:
                return "FOR";
            case IF:
                return "IF";
            case IMPORT:
                return "IMPORT";
            case INT:
                return "INT";
            case INTVAL:
                return "INTVAL";
            case LAMBDA:
                return "LAMBDA";
            case LCURLY:
                return "LCURLY";
            case LPAREN:
                return "LPAREN";
            case LSQUARE:
                return "LSQUARE";
            case OP:
                return "OP";
            case PIPE:
                return "PIPE";
            case QUESTION:
                return "QUESTION";
            case RCURLY:
                return "RCURLY";
            case RPAREN:
                return "RPAREN";
            case RSQUARE:
                return "RSQUARE";
            case SEMICOLON:
                return "SEMICOLON";
            case STRING:
                return "STRING";
            case STRINGLITERAL:
                return "STRINGLITERAL";
            case STRINGVAL:
                return "STRINGVAL";
            case TRUE:
                return "TRUE";
            case TYPE:
                return "TYPE";
            case VARIABLE:
                return "VARIABLE";
            case WHILE:
                return "WHILE";
            default:
                return "UNKNOWN";
        }
    }

    std::string FilePosition::toString() const {
        return *fileName + ":" + std::to_string(line) + ":" + std::to_string(column);
    }

    std::string Token::toString() const { return tokenTypeToString(type) + " '" + contents + "'"; }

    std::string StringToken::toString() const {
        std::ostringstream builder;
        for (const Token token : subTokens) builder << token.toString() << "\n";
        return tokenTypeToString(type) + "\n[\n" + builder.str() + "]";
    }

    std::string readFile(const std::string& filePath) {
        std::ifstream in(filePath);
        if (!in.is_open()) { throw Util::CommanderException("File not found at " + filePath); }
        std::string expectedOutput = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        in.close();
        return expectedOutput;
    }

    void tokenize(TokenList& tokens, const std::string& filePath) {
        const std::string file = readFile(filePath);
        const std::shared_ptr<std::string> filePathShared = std::make_shared<std::string>(filePath);
        FilePosition position = {filePathShared, 1, 1, 0};
        while (position.index < file.length()) {
            skipWhitespace(file, position);
            int start = position.index;
            Token token = lexToken(file, position);
            if (start != position.index) {
                tokens.push_back(token);
                continue;
            }
            if (isIllegalCharacter(file[position.index])) {
                throw Util::CommanderException(
                        "Illegal character (ascii " + std::to_string((int)(file[position.index])) + ")", position);
            }
            throw Util::CommanderException("Unrecognized token", position);
        }
    }

    FilePosition skipWhitespace(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    Token lexToken(const std::string& file, FilePosition& position) {
        // TODO: implement
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