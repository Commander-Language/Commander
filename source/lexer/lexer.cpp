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
            case BREAK:
                return "BREAK";
            case CMDSTRINGVAL:
                return "CMDSTRINGVAL";
            case COLON:
                return "COLON";
            case COMMA:
                return "COMMA";
            case CONST:
                return "CONST";
            case CONTINUE:
                return "CONTINUE";
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

    Token::Token() : contents(""), type(UNKNOWN), position({std::make_shared<std::string>(""), -1, -1, -1}) {}
    Token::Token(std::string c, TokenType t, FilePosition p) : contents(c), type(t), position(p) {}

    std::string Token::toString() const {
        return type != END_OF_FILE ? tokenTypeToString(type) + " '" + contents + "'" : tokenTypeToString(type);
    }

    std::string StringToken::toString() const {
        std::ostringstream builder;
        for (const Token& token : subTokens) builder << token.toString() << "\n";
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
        skipWhitespace(file, position);
        while (position.index < file.length()) {
            Token token = lexToken(file, position);
            if (token.type != UNKNOWN) {
                tokens.push_back(token);
                skipWhitespace(file, position);
                continue;
            }
            if (isIllegalCharacter(file[position.index])) {
                throw Util::CommanderException(
                        "Illegal character (ascii " + std::to_string((int)(file[position.index])) + ")", position);
            }
            throw Util::CommanderException("Unrecognized token", position);
        }
        // ASCII character 5 is the EOF character
        tokens.push_back({std::string(1, (char)5), END_OF_FILE, position});
    }

    void skipWhitespace(const std::string& file, FilePosition& position) {
        bool isLineComment = false;
        bool isBlockComment = false;
        FilePosition blockCommentPosition;
        while (position.index < file.length()) {
            char character = file[position.index];
            if (character == '\n') {
                if (isLineComment) { isLineComment = false; }
                position.line++;
                position.column = 1;
                position.index++;
                continue;
            }
            // Ignore all characters in a comment (except '*' in block comments) as well as spaces and \'s
            if (isLineComment || (isBlockComment && character != '*') || isWhitespace(character)) {
                position.index++;
                position.column++;
                continue;
            }
            char nextCharacter = position.index + 1 < file.length() ? file[position.index + 1] : '\0';
            // At this point, the character is not whitespace, so if it is not a comment (or end of block comment),
            // return
            if (!isBlockComment && (character != '/' || (nextCharacter != '/' && nextCharacter != '*'))) { return; }
            // Determine if there is a comment that is being started or terminated
            bool terminateBlockComment = isBlockComment && nextCharacter == '/';
            bool startBlockComment = character == '/' && nextCharacter == '*';
            bool startLineComment = character == '/' && nextCharacter == '/';
            if (terminateBlockComment) { isBlockComment = false; }
            if (startBlockComment) {
                isBlockComment = true;
                blockCommentPosition = position;
            }
            if (startLineComment) { isLineComment = true; }
            if (terminateBlockComment || startBlockComment || startLineComment) {
                position.index += 2;
                position.column += 2;
            }
            // If a comment is started incorrectly, throw an error
            if (character == '/' && !startLineComment && !startBlockComment) {
                throw Util::CommanderException("Expected either '*' or '/' after '/'", position);
            }
            position.index++;
            position.column++;
        }
        if (isBlockComment) { throw Util::CommanderException("Unterminated block comment", blockCommentPosition); }
    }

    Token lexToken(const std::string& file, FilePosition& position) {
        Token token = lexTokenLiteral(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexKeyword(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexFloat(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexInt(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexString(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexVariable(file, position);
        if (token.type != UNKNOWN) return token;
        token = lexCommandString(file, position);
        if (token.type != UNKNOWN) return token;
        return Token();
    }

    Token lexTokenLiteral(const std::string& file, FilePosition& position) {
        for (const std::pair<std::string, TokenType>& literal : TOKEN_LITERALS) {
            int length = literal.first.length();
            if (position.index + length >= file.length()
                || literal.first != std::string(file, position.index, length)) {
                continue;
            }
            Token token = {literal.first, literal.second, position};
            position.index += length;
            position.column += length;
            return token;
        }
        return Token();
    }

    Token lexKeyword(const std::string& file, FilePosition& position) {
        for (const std::pair<std::string, TokenType>& keyword : KEYWORDS) {
            int length = keyword.first.length();
            if (position.index + length >= file.length() || keyword.first != std::string(file, position.index, length)
                || (position.index + length + 1 < file.length()
                    && !isVariableCharacter(file[position.index + length]))) {
                continue;
            }
            Token token = {keyword.first, keyword.second, position};
            position.index += length;
            position.column += length;
            return token;
        }
        return Token();
    }

    Token lexFloat(const std::string& file, FilePosition& position) {
        bool hasDigit = false;
        bool hasDot = false;
        int index = position.index;
        while (index < file.length()) {
            char character = file[index];
            if (isDigit(character)) {
                hasDigit = true;
            } else if (character == '.' && !hasDot) {
                hasDot = true;
            } else {
                break;
            }
            index++;
        }
        if (hasDot && hasDigit) {
            int length = index - position.index;
            Token token = {std::string(file, position.index, length), FLOATVAL, position};
            position.index += length;
            position.column += length;
            return token;
        }
        return Token();
    }

    Token lexInt(const std::string& file, FilePosition& position) {
        // The first character must be a digit
        if (!isDigit(file[position.index])) { return Token(); }
        // The token is definitely an int (assuming float has already been tried), so determine the length/contents
        FilePosition startPosition = position;
        do {
            position.index++;
            position.column++;
        } while (position.index < file.length() && isDigit(file[position.index]));
        return {std::string(file, startPosition.index, position.index - startPosition.index), INTVAL, startPosition};
    }

    Token lexString(const std::string& file, FilePosition& position) {
        bool isFormat = file[position.index] == '$';
        bool isSingle = file[position.index] == '\'';
        bool isDouble = file[position.index] == '"';
        if (isFormat) {
            bool secondExists = position.index + 1 < file.length();
            isSingle = secondExists && file[position.index + 1] == '\'';
            isDouble = secondExists && file[position.index + 1] == '"';
        }
        if (!isSingle && !isDouble) { return Token(); }
        StringToken token;
        token.position = position;
        token.type = STRINGVAL;
        token.contents = "";
        position.index += isFormat ? 2 : 1;
        position.column += isFormat ? 2 : 1;
        bool stringTerminated = false;
        std::ostringstream currentString;
        FilePosition currentStringPosition = position;
        // The token is definitely a string, so determine the length/contents
        while (position.index < file.length()) {
            char character = file[position.index++];
            position.column++;
            // Ensure the string contains no illegal characters
            if (isIllegalCharacter(character)) {
                throw Util::CommanderException(
                        "String contains illegal character (ascii " + std::to_string((int)character) + ")", position);
            }
            // Check if string is terminated
            if ((isSingle && character == '\'') || (isDouble && character == '"')) {
                stringTerminated = true;
                std::string s = currentString.str();
                if (s != "") { token.subTokens.push_back({currentString.str(), STRINGLITERAL, currentStringPosition}); }
                break;
            }
            // Break out so that it can throw string not terminated error if it is at end of file
            if (position.index >= file.length()) { break; }
            // Handle escape characters
            if (character == '\\') {
                char secondCharacter = file[position.index++];
                position.column++;
                if (isFormat && secondCharacter == '{') {
                    currentString << '{';
                    continue;
                }
                if (isFormat && secondCharacter == '}') {
                    currentString << '}';
                    continue;
                }
                if (secondCharacter == '\\') {
                    currentString << '\\';
                    continue;
                }
                if (isSingle) {
                    if (secondCharacter == '\'') {
                        currentString << '\'';
                    } else {
                        throw Util::CommanderException(
                                "Unknown escape sequence",
                                {position.fileName, position.line, position.index - 2, position.index - 2});
                    }
                    continue;
                }
                switch (secondCharacter) {
                    case '"':
                        currentString << '"';
                        break;
                    case 'n':
                        currentString << '\n';
                        break;
                    case 't':
                        currentString << '\t';
                        break;
                    case 'r':
                        currentString << '\r';
                        break;
                    case '$':
                        currentString << '$';
                        break;
                    case '\r':  // If a backslash appears before newline, don't include newline in resulting string
                    case '\n':
                        if (secondCharacter == '\r' && position.index < file.length() && file[position.index] == '\n') {
                            position.index++;
                        }
                        position.column = 1;
                        position.line++;
                        break;
                    default:
                        throw Util::CommanderException(
                                "Unknown escape sequence",
                                {position.fileName, position.line, position.index - 2, position.index - 2});
                }
                continue;
            }
            // Handle variables
            if (character == '$' && isDouble && isFirstVariableCharacter(file[position.index])) {
                std::string s = currentString.str();
                if (s != "") {
                    token.subTokens.push_back({currentString.str(), STRINGLITERAL, currentStringPosition});
                    currentString.str("");
                }
                token.subTokens.push_back(lexVariable(file, position));
                currentStringPosition = position;
                continue;
            }
            // Handle formats
            if ((character == '$' && file[position.index] == '{' && isDouble && !isFormat)
                || (character == '{' && isFormat)) {
                if (!isFormat) {
                    position.index++;
                    position.column++;
                }
                std::string s = currentString.str();
                if (s != "") {
                    token.subTokens.push_back({currentString.str(), STRINGLITERAL, currentStringPosition});
                    currentString.str("");
                }
                FilePosition startPosition = position;
                // TODO: This part is very similar to tokenize, so possibly create helper or something to reduce code
                skipWhitespace(file, position);
                while (position.index < file.length()) {
                    Token t = lexToken(file, position);
                    if (t.type != UNKNOWN) {
                        if (t.type == RCURLY) { break; }
                        token.subTokens.push_back(t);
                        skipWhitespace(file, position);
                        continue;
                    }
                    if (isIllegalCharacter(file[position.index])) {
                        throw Util::CommanderException("Illegal character (ascii "
                                                               + std::to_string((int)(file[position.index])) + ")",
                                                       position);
                    }
                    throw Util::CommanderException("Unrecognized token", position);
                }
                if (file[position.index - 1] != '}') {
                    throw Util::CommanderException("Unterminated format expression in string", startPosition);
                }
                currentStringPosition = position;
                continue;
            }
            currentString << character;
        }
        if (!stringTerminated) {
            throw Util::CommanderException("String wasn't terminated with " + std::string((isSingle ? "'" : "\"")),
                                           token.position);
        }
        return token;
    }

    Token lexVariable(const std::string& file, FilePosition& position) {
        // First character may optionally be a $, but it will be ignored
        bool startsWithDollarSign = file[position.index] == '$';
        if (startsWithDollarSign) {
            position.index++;
            position.column++;
        }
        // First character of variable must be a letter or an underscore
        if (position.index >= file.length() || !isFirstVariableCharacter(file[position.index])) {
            if (position.index >= file.length() || startsWithDollarSign) {
                throw Util::CommanderException("Variable does not start with a letter or underscore");
            }
            return Token();
        }
        // Token is definitely a variable, so determine length/contents
        std::ostringstream builder;
        FilePosition startPosition = position;
        while (position.index < file.length() && isVariableCharacter(file[position.index])) {
            builder << file[position.index++];
            position.column++;
        }
        return {std::string(builder.str()), VARIABLE, startPosition};
    }

    Token lexCommandString(const std::string& file, FilePosition& position) {
        // TODO: Implement
        throw new std::exception();
    }

    bool isFirstVariableCharacter(const char& character) {
        return isVariableCharacter(character) && !isDigit(character);
    }

    bool isVariableCharacter(const char& character) {
        return isLetter(character) || isDigit(character) || character == '_';
    }

    bool isLetter(const char& character) {
        return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z');
    }

    bool isDigit(const char& character) { return character >= '0' && character <= '9'; }

    bool isWhitespace(const char& character) {
        return character == '\n' || character == '\r' || character == '\t' || character == ' ';
    }

    bool isIllegalCharacter(const char& character) {
        int ascii = (int)character;
        return !isWhitespace(character) && (ascii < 32 || ascii > 126);
    }

}  // namespace Lexer