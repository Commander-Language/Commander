/**
 * @file lexer.cpp
 * @brief Contains implementation for functions in lexer.hpp
 *
 */

#include "lexer.hpp"
#include "../util/commander_exception.hpp"

namespace lexer {

    std::string tokenTypeToString(const tokenType& type) {
        switch (type) {
            case ALIAS:
                return "ALIAS";
            case AMPERSAND:
                return "AMPERSAND";
            case BACKTICK:
                return "BACKTICK";
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

    Token::Token() : type(UNKNOWN), position({std::make_shared<std::string>(""), -1, -1, -1}) {}

    Token::Token(std::string contents, tokenType type, FilePosition position)
        : contents(std::move(contents)), type(type), position(std::move(position)) {}

    std::string Token::toString() const {
        return type != END_OF_FILE ? tokenTypeToString(type) + " '" + contents + "'" : tokenTypeToString(type);
    }

    std::string StringToken::toString() const {
        std::ostringstream builder;
        for (const TokenPtr& token : subTokens) builder << token->toString() << "\n";
        return tokenTypeToString(type) + "\n[\n" + builder.str() + "]";
    }

    std::string readFile(const std::string& filePath) {
        std::ifstream input(filePath);
        if (!input.is_open()) { throw util::CommanderException("File not found at " + filePath); }
        std::string expectedOutput = std::string(std::istreambuf_iterator<char>(input),
                                                 std::istreambuf_iterator<char>());
        input.close();
        return expectedOutput;
    }

    void tokenize(TokenList& tokens, const std::string& filePath) {
        const std::string file = readFile(filePath);
        const std::shared_ptr<std::string> filePathShared = std::make_shared<std::string>(filePath);
        FilePosition position = {filePathShared, 1, 1, 0};
        bool isCommand = false;
        bool isBacktickCommand = false;
        bool isFirst = true;
        FilePosition commandPosition;
        skipWhitespace(file, position);
        while (position.index < file.length()) {
            const TokenPtr token = lexToken(file, position, isCommand, isFirst);
            if (!token) {
                if (isIllegalCharacter(file[position.index])) {
                    throw util::CommanderException(
                            "Illegal character (ascii " + std::to_string((int)(file[position.index])) + ")", position);
                }
                throw util::CommanderException("Unrecognized token", position);
            }
            if (token->type == SEMICOLON) {
                isCommand = false;
                isFirst = true;
            }
            if (isCommand && isFirst) { commandPosition = token->position; }
            if (token->type == BACKTICK) {
                if (!isCommand) {
                    commandPosition = token->position;
                    isCommand = true;
                    isBacktickCommand = true;
                } else if (!isBacktickCommand) {
                    throw util::CommanderException("Cannot use backticks in command", token->position);
                } else {
                    isCommand = false;
                    isBacktickCommand = false;
                }
            }
            if (isFirst && token->type != SEMICOLON) { isFirst = false; }
            tokens.push_back(token);
            skipWhitespace(file, position);
        }
        if (isCommand && isBacktickCommand) {
            throw util::CommanderException("Command was not terminated with a backtick", commandPosition);
        }
        if (isCommand) {
            throw util::CommanderException("Command was not terminated with a semicolon", commandPosition);
        }
        // ASCII character 5 is the EOF character
        const char endOfFile = (char)5;
        const Token eofToken = {std::string(1, endOfFile), END_OF_FILE, position};
        tokens.push_back(std::make_shared<Token>(eofToken));
    }

    void skipWhitespace(const std::string& file, FilePosition& position) {
        bool isLineComment = false;
        bool isBlockComment = false;
        FilePosition blockCommentPosition;
        while (position.index < file.length()) {
            const char character = file[position.index];
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
            const char nextCharacter = position.index + 1 < file.length() ? file[position.index + 1] : '\0';
            // At this point, the character is not whitespace, so if it is not a comment (or end of block comment),
            // return
            if (!isBlockComment && (character != '/' || (nextCharacter != '/' && nextCharacter != '*'))) { return; }
            // Determine if there is a comment that is being started or terminated
            const bool terminateBlockComment = isBlockComment && nextCharacter == '/';
            const bool startBlockComment = character == '/' && nextCharacter == '*';
            const bool startLineComment = character == '/' && nextCharacter == '/';
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
                throw util::CommanderException("Expected either '*' or '/' after '/'", position);
            }
            position.index++;
            position.column++;
        }
        if (isBlockComment) { throw util::CommanderException("Unterminated block comment", blockCommentPosition); }
    }

    TokenPtr lexToken(const std::string& file, FilePosition& position, bool& isCommand, const bool& isFirst) {
        TokenPtr token;
        if (!isCommand) {
            token = lexTokenLiteral(file, position);
            if (token) return token;
        }
        token = lexCommandTokenLiteral(file, position);
        if (token) return token;
        if (!isCommand) {
            token = lexKeyword(file, position);
            if (token) return token;
            token = lexFloat(file, position);
            if (token) return token;
            token = lexInt(file, position);
            if (token) return token;
        }
        token = lexString(file, position);
        if (token) {
            if (isFirst && token->type == STRINGVAL) isCommand = true;
            return token;
        }
        token = lexCommandVariable(file, position);
        if (token) {
            if (isFirst && token->type == VARIABLE) isCommand = true;
            return token;
        }
        if (!isCommand) {
            token = lexVariable(file, position);
            if (token) return token;
        }
        token = lexCommandString(file, position);
        if (token) {
            if (isFirst && token->type == CMDSTRINGVAL) isCommand = true;
            return token;
        }
        return {};
    }

    TokenPtr lexTokenLiteral(const std::string& file, FilePosition& position) {
        for (const auto& literal : tokenLiterals) {
            const int length = (int)literal.first.length();
            if (position.index + length > file.length() || literal.first != std::string(file, position.index, length)) {
                continue;
            }
            const Token token = {literal.first, literal.second, position};
            position.index += length;
            position.column += length;
            return std::make_shared<Token>(token);
        }
        return {};
    }

    TokenPtr lexCommandTokenLiteral(const std::string& file, FilePosition& position) {
        // TODO(phales): Very similar to lexTokenLiteral, perhaps create a helper method to improve code reuse
        for (const auto& literal : commandTokenLiterals) {
            const int length = (int)literal.first.length();
            const int fileLength = (int)file.length();
            const std::string tmp = std::string(file, position.index, length);
            const std::string tmp2 = literal.first;
            if (position.index + length > fileLength || tmp2 != tmp) { continue; }
            const Token token = {literal.first, literal.second, position};
            position.index += length;
            position.column += length;
            return std::make_shared<Token>(token);
        }
        return {};
    }

    TokenPtr lexKeyword(const std::string& file, FilePosition& position) {
        // TODO(phales): Very similar to lexTokenLiteral, perhaps create a helper method to improve code reuse
        for (const auto& keyword : keywords) {
            const int length = (int)keyword.first.length();
            if (position.index + length > file.length() || keyword.first != std::string(file, position.index, length)
                || (position.index + length + 1 < file.length()
                    && !isVariableCharacter(file[position.index + length]))) {
                continue;
            }
            const Token token = {keyword.first, keyword.second, position};
            position.index += length;
            position.column += length;
            return std::make_shared<Token>(token);
        }
        return {};
    }

    TokenPtr lexFloat(const std::string& file, FilePosition& position) {
        bool hasDigit = false;
        bool hasDot = false;
        int index = position.index;
        while (index < file.length()) {
            const char character = file[index];
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
            const int length = index - position.index;
            const Token token = {std::string(file, position.index, length), FLOATVAL, position};
            position.index += length;
            position.column += length;
            return std::make_shared<Token>(token);
        }
        return {};
    }

    TokenPtr lexInt(const std::string& file, FilePosition& position) {
        // The first character must be a digit
        if (!isDigit(file[position.index])) { return {}; }
        // The token is definitely an int (assuming float has already been tried), so determine the length/contents
        const FilePosition startPosition = position;
        while (position.index < file.length() && isDigit(file[position.index])) {
            position.index++;
            position.column++;
        }
        const Token token = {std::string(file, startPosition.index, position.index - startPosition.index), INTVAL,
                             startPosition};
        return std::make_shared<Token>(token);
    }

    TokenPtr lexString(const std::string& file, FilePosition& position) {
        const bool isFormat = file[position.index] == '$';
        bool isSingle = file[position.index] == '\'';
        bool isDouble = file[position.index] == '"';
        if (isFormat) {
            const bool secondExists = position.index + 1 < file.length();
            isSingle = secondExists && file[position.index + 1] == '\'';
            isDouble = secondExists && file[position.index + 1] == '"';
        }
        if (!isSingle && !isDouble) { return {}; }
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
            const char character = file[position.index++];
            position.column++;
            // Ensure the string contains no illegal characters
            if (isIllegalCharacter(character)) {
                throw util::CommanderException(
                        "String contains illegal character (ascii " + std::to_string((int)character) + ")", position);
            }
            // Check if string is terminated
            if ((isSingle && character == '\'') || (isDouble && character == '"')) {
                stringTerminated = true;
                const std::string str = currentString.str();
                if (!str.empty()) {
                    const Token strToken = {currentString.str(), STRINGLITERAL, currentStringPosition};
                    token.subTokens.push_back(std::make_shared<Token>(strToken));
                }
                break;
            }
            // Break out so that it can throw string not terminated error if it is at end of file
            if (position.index >= file.length()) { break; }
            // Handle escape characters
            if (character == '\\') {
                const char secondCharacter = file[position.index++];
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
                        throw util::CommanderException(
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
                    case '\n':
                    case '\r':  // If a backslash appears before newline, don't include newline in resulting string
                        if (secondCharacter == '\r' && position.index < file.length() && file[position.index] == '\n') {
                            position.index++;
                        }
                        position.column = 1;
                        position.line++;
                        break;
                    default:
                        throw util::CommanderException(
                                "Unknown escape sequence",
                                {position.fileName, position.line, position.index - 2, position.index - 2});
                }
                continue;
            }
            // Handle variables
            if (character == '$' && isDouble && isFirstVariableCharacter(file[position.index])) {
                const std::string str = currentString.str();
                if (!str.empty()) {
                    const Token strToken = {currentString.str(), STRINGLITERAL, currentStringPosition};
                    token.subTokens.push_back(std::make_shared<Token>(strToken));
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
                const std::string str = currentString.str();
                if (!str.empty()) {
                    const Token strToken = {currentString.str(), STRINGLITERAL, currentStringPosition};
                    token.subTokens.push_back(std::make_shared<Token>(strToken));
                    currentString.str("");
                }
                const FilePosition startPosition = {position.fileName, position.line, position.column - 1,
                                                    position.index - 1};
                // TODO(phales): This part is very similar to tokenize, so possibly create helper or something to reduce
                // code
                bool isCommand = false;
                FilePosition commandPosition;
                skipWhitespace(file, position);
                while (position.index < file.length()) {
                    const TokenPtr tok = lexToken(file, position, isCommand, false);
                    if (!tok) {
                        if (isIllegalCharacter(file[position.index])) {
                            throw util::CommanderException("Illegal character (ascii "
                                                                   + std::to_string((int)(file[position.index])) + ")",
                                                           position);
                        }
                        throw util::CommanderException("Unrecognized token", position);
                    }
                    if (tok->type == BACKTICK) {
                        if (!isCommand) {
                            commandPosition = tok->position;
                            isCommand = true;
                        } else {
                            isCommand = false;
                        }
                    }
                    if (tok->type == RCURLY) { break; }
                    token.subTokens.push_back(tok);
                    skipWhitespace(file, position);
                }
                if (isCommand) {
                    throw util::CommanderException("Command was not terminated with a backtick", commandPosition);
                }
                if (file[position.index - 1] != '}') {
                    throw util::CommanderException("Unterminated format expression in string", startPosition);
                }
                currentStringPosition = position;
                continue;
            }
            currentString << character;
        }
        if (!stringTerminated) {
            throw util::CommanderException("String wasn't terminated with " + std::string((isSingle ? "'" : "\"")),
                                           token.position);
        }
        return std::make_shared<StringToken>(token);
    }

    TokenPtr lexCommandVariable(const std::string& file, FilePosition& position) {
        // First character must be $
        if (file[position.index] != '$') { return {}; }
        // Next character of variable must be a letter or an underscore
        if (position.index + 1 >= file.length() || !isFirstVariableCharacter(file[position.index + 1])) { return {}; }
        position.index++;
        position.column++;
        return lexVariable(file, position);
    }

    TokenPtr lexVariable(const std::string& file, FilePosition& position) {
        // First character of variable must be a letter or an underscore
        if (!isFirstVariableCharacter(file[position.index])) { return {}; }
        // Token is definitely a variable, so determine length/contents
        std::ostringstream builder;
        const FilePosition startPosition = position;
        while (position.index < file.length() && isVariableCharacter(file[position.index])) {
            builder << file[position.index++];
            position.column++;
        }
        const Token token = {std::string(builder.str()), VARIABLE, startPosition};
        return std::make_shared<Token>(token);
    }

    TokenPtr lexCommandString(const std::string& file, FilePosition& position) {
        std::ostringstream builder;
        const FilePosition startPosition = position;
        while (position.index < file.length()) {
            const char character = file[position.index];
            if (isWhitespace(character)
                || (commandTokenLiterals.find(std::string(1, character)) != commandTokenLiterals.end())) {
                break;
            }
            builder << character;
            position.index++;
            position.column++;
        }
        const Token token = {std::string(builder.str()), CMDSTRINGVAL, startPosition};
        return std::make_shared<Token>(token);
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
        // ' ' is ascii 32, and '~' is ascii 126. Anything below or above these, that isn't whitespace, is illegal
        return !isWhitespace(character) && (character < ' ' || character > '~');
    }

}  // namespace lexer