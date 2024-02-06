/**
 * @file lexer.cpp
 * @brief Contains implementation for functions in lexer.hpp
 *
 */

#include <fstream>
#include <sstream>

#include "lexer.hpp"
#include "source/util/commander_exception.hpp"

namespace Lexer {

    std::string tokenTypeToString(const tokenType& type) {
        switch (type) {
            case ADD:
                return "ADD";
            case ADD_EQUALS:
                return "ADD_EQUALS";
            case ALIAS:
                return "ALIAS";
            case AMPERSAND:
                return "AMPERSAND";
            case AND:
                return "AND";
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
            case DECREMENT:
                return "DECREMENT";
            case DIVIDE:
                return "DIVIDE";
            case DIVIDE_EQUALS:
                return "DIVIDE_EQUALS";
            case DO:
                return "DO";
            case DOT:
                return "DOT";
            case DOUBLE_EQUALS:
                return "DOUBLE_EQUALS";
            case ELSE:
                return "ELSE";
            case END_OF_FILE:
                return "END_OF_FILE";
            case EXPONENTIATE:
                return "EXPONENTIATE";
            case EXPONENTIATE_EQUALS:
                return "EXPONENTIATE_EQUALS";
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
            case GREATER:
                return "GREATER";
            case GREATER_EQUAL:
                return "GREATER_EQUAL";
            case IF:
                return "IF";
            case IMPORT:
                return "IMPORT";
            case INCREMENT:
                return "INCREMENT";
            case INT:
                return "INT";
            case INTVAL:
                return "INTVAL";
            case LAMBDA:
                return "LAMBDA";
            case LCURLY:
                return "LCURLY";
            case LESSER:
                return "LESSER";
            case LESSER_EQUAL:
                return "LESSER_EQUAL";
            case LPAREN:
                return "LPAREN";
            case LSQUARE:
                return "LSQUARE";
            case MINUS:
                return "MINUS";
            case MINUS_EQUALS:
                return "MINUS_EQUALS";
            case MODULO:
                return "MODULO";
            case MODULO_EQUALS:
                return "MODULO_EQUALS";
            case MULTIPLY:
                return "MULTIPLY";
            case MULTIPLY_EQUALS:
                return "MULTIPLY_EQUALS";
            case NOT:
                return "NOT";
            case NOT_EQUALS:
                return "NOT_EQUALS";
            case OR:
                return "OR";
            case PIPE:
                return "PIPE";
            case PRINT:
                return "PRINT";
            case PRINTLN:
                return "PRINTLN";
            case QUESTION:
                return "QUESTION";
            case RCURLY:
                return "RCURLY";
            case READ:
                return "READ";
            case RETURN:
                return "RETURN";
            case RPAREN:
                return "RPAREN";
            case RSQUARE:
                return "RSQUARE";
            case SCAN:
                return "SCAN";
            case SEMICOLON:
                return "SEMICOLON";
            case STRING:
                return "STRING";
            case STRINGLITERAL:
                return "STRINGLITERAL";
            case STRINGVAL:
                return "STRINGVAL";
            case TIMEOUT:
                return "TIMEOUT";
            case TO:
                return "TO";
            case TRUE:
                return "TRUE";
            case TYPE:
                return "TYPE";
            case VARIABLE:
                return "VARIABLE";
            case WHILE:
                return "WHILE";
            case WRITE:
                return "WRITE";
            default:
                return "UNKNOWN";
        }
    }

    std::string FilePosition::toString() const {
        return fileName + ":" + std::to_string(line) + ":" + std::to_string(column);
    }

    Token::Token() : type(UNKNOWN), position({"", -1, -1, -1}) {}

    Token::Token(std::string contents, tokenType type, FilePosition position)
        : contents(std::move(contents)), type(type), position(std::move(position)) {}

    std::string Token::toString() const {
        return (type != END_OF_FILE ? tokenTypeToString(type) + " '" + contents + "'" : tokenTypeToString(type)) + " "
             + std::to_string(position.line) + ":" + std::to_string(position.column);
    }

    std::string StringToken::toString() const {
        std::ostringstream builder;
        for (const TokenPtr& token : subTokens) builder << token->toString() << "\n";
        return tokenTypeToString(type) + " " + std::to_string(position.line) + ":" + std::to_string(position.column)
             + "\n[\n" + builder.str() + "]";
    }

    std::string readFile(const std::string& filePath) {
        std::ifstream input(filePath);
        if (!input.is_open()) { throw Util::CommanderException("File not found at " + filePath); }
        return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    }

    void tokenize(TokenList& tokens, const std::string& filePath) {
        const std::string file = readFile(filePath);
        FilePosition position = {filePath, 1, 1, 0};
        skipWhitespace(file, position);
        while (position.index < file.length()) {
            lexStatement(tokens, file, position, SEMICOLON);
            skipWhitespace(file, position);
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
            if (character == '\n' || character == '\r') {
                if (isLineComment) { isLineComment = false; }
                position.line++;
                position.column = 1;
                position.index++;
                if (position.index < file.length() && character == '\r' && file[position.index] == '\n') {
                    position.index++;
                }
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
            } else {
                position.index++;
                position.column++;
            }
            // If a comment is started incorrectly, throw an error
            if (character == '/' && !startLineComment && !startBlockComment) {
                throw Util::CommanderException("Expected either '*' or '/' after '/'", position);
            }
        }
        if (isBlockComment) { throw Util::CommanderException("Unterminated block comment", blockCommentPosition); }
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
        if (isIllegalCharacter(file[position.index])) {
            throw Util::CommanderException(
                    "Illegal character (ascii " + std::to_string((int)(file[position.index])) + ")", position);
        }
        throw Util::CommanderException("Unrecognized token", position);
    }

    TokenPtr
    lexLiteral(const std::string& file, FilePosition& position,
               const std::vector<std::pair<std::basic_string<char>, tokenType>>& literals,
               const std::function<bool(const std::pair<std::basic_string<char>, tokenType>& literal,
                                        const std::string& file, FilePosition& position)>& notLiteralCondition) {
        for (const auto& literal : literals) {
            const int length = (int)literal.first.length();
            if (notLiteralCondition(literal, file, position)) { continue; }
            const Token token = {literal.first, literal.second, position};
            position.index += length;
            position.column += length;
            return std::make_shared<Token>(token);
        }
        return {};
    }

    TokenPtr lexTokenLiteral(const std::string& file, FilePosition& position) {
        return lexLiteral(file, position, tokenLiterals,
                          [](const std::pair<std::basic_string<char>, tokenType>& literal, const std::string& file,
                             FilePosition& position) {
                              const int length = (int)literal.first.length();
                              return position.index + length > file.length()
                                  || literal.first != std::string(file, position.index, length);
                          });
    }

    TokenPtr lexCommandTokenLiteral(const std::string& file, FilePosition& position) {
        return lexLiteral(file, position, commandTokenLiterals,
                          [](const std::pair<std::basic_string<char>, tokenType>& literal, const std::string& file,
                             FilePosition& position) {
                              const int length = (int)literal.first.length();
                              const int fileLength = (int)file.length();
                              const std::string word = std::string(file, position.index, length);
                              const std::string literalWord = literal.first;
                              return position.index + length > fileLength || literalWord != word;
                          });
    }

    TokenPtr lexKeyword(const std::string& file, FilePosition& position) {
        return lexLiteral(file, position, keywords,
                          [](const std::pair<std::basic_string<char>, tokenType>& literal, const std::string& file,
                             FilePosition& position) {
                              const int length = (int)literal.first.length();
                              return position.index + length > file.length()
                                  || literal.first != std::string(file, position.index, length)
                                  || (position.index + length + 1 < file.length()
                                      && isVariableCharacter(file[position.index + length]));
                          });
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
            // Ignore tab characters in strings
            if (character == '\t') { continue; }
            // Ensure the string contains no illegal characters
            if (isIllegalCharacter(character)) {
                throw Util::CommanderException(
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
            // Handle new lines
            if (character == '\r' || character == '\n') {
                const char secondCharacter = file[position.index++];
                position.column = 1;
                position.line++;
                if (character == '\n' || (character == '\r' && secondCharacter != '\n')) { position.index--; }
                currentString << character;
                if (character == '\r' && secondCharacter == '\n') { currentString << secondCharacter; }
                continue;
            }
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
                if (secondCharacter == '\n' || secondCharacter == '\r') {
                    if (secondCharacter == '\r' && position.index < file.length() && file[position.index] == '\n') {
                        position.index++;
                    }
                    position.column = 1;
                    position.line++;
                    continue;
                }
                if (isSingle) {
                    if (secondCharacter == '\'') {
                        currentString << '\'';
                    } else {
                        throw Util::CommanderException(
                                "Unknown escape sequence \\" + std::string(1, secondCharacter),
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
                    default:
                        throw Util::CommanderException(
                                "Unknown escape sequence \\" + std::string(1, secondCharacter),
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
                lexExpression(token.subTokens, file, position, RCURLY);
                token.subTokens.pop_back();
                currentStringPosition = position;
                continue;
            }
            currentString << character;
        }
        if (!stringTerminated) {
            throw Util::CommanderException("String wasn't terminated with " + std::string((isSingle ? "'" : "\"")),
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
            if (isIllegalCharacter(character)) { return {}; }
            if (isWhitespace(character)) { break; }
            const std::string characterString = std::string(1, character);
            for (const auto& literal : commandTokenLiterals) {
                if (characterString == literal.first) {
                    const Token token = {std::string(builder.str()), CMDSTRINGVAL, startPosition};
                    return std::make_shared<Token>(token);
                }
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

    TokenPtr expectToken(const tokenType& type, const std::string& file, FilePosition& position, bool& isCommand) {
        skipWhitespace(file, position);
        if (position.index >= file.length()) {
            throw Util::CommanderException("Expected " + tokenTypeToString(type) + " token, but file ended.", position);
        }
        const TokenPtr token = lexToken(file, position, isCommand, false);
        if (token->type != type) {
            throw Util::CommanderException("Expected " + tokenTypeToString(type) + " token, but found "
                                                   + tokenTypeToString(token->type) + " token.",
                                           token->position);
        }
        return token;
    }

    void lexStatement(TokenList& tokens, const std::string& file, FilePosition& position,
                      const tokenType& terminatingToken) {
        bool isCommand = false;
        bool isBacktickCommand = false;
        bool isFirst = true;
        const FilePosition startPosition = position;
        FilePosition commandPosition;
        skipWhitespace(file, position);
        while (position.index < file.length()) {
            const TokenPtr token = lexToken(file, position, isCommand, isFirst);
            tokens.push_back(token);
            if (token->type == terminatingToken && isBacktickCommand) { break; }
            if (token->type == terminatingToken) { return; }
            // Lex scope
            if (token->type == LCURLY) {
                skipWhitespace(file, position);
                while (position.index < file.length()) {
                    if (file[position.index] == '}') {
                        tokens.push_back(expectToken(RCURLY, file, position, isCommand));
                        return;
                    }
                    lexStatement(tokens, file, position, SEMICOLON);
                    skipWhitespace(file, position);
                }
                throw Util::CommanderException("Unterminated scope", token->position);
            }
            if (isCommand && isFirst) { commandPosition = token->position; }
            if (token->type == BACKTICK) {
                if (!isCommand) {
                    commandPosition = token->position;
                    isCommand = true;
                    isBacktickCommand = true;
                } else if (!isBacktickCommand) {
                    throw Util::CommanderException("Cannot use backticks in command", token->position);
                } else {
                    isCommand = false;
                    isBacktickCommand = false;
                }
            }
            // Look ahead for variables
            if (token->type == VARIABLE && isFirst && !isCommand) {
                // Get the next token
                const int indexBeforeSkip = position.index;
                skipWhitespace(file, position);
                if (position.index >= file.length()) {
                    isCommand = true;
                    break;
                }
                const bool noSpace = indexBeforeSkip == position.index;
                const TokenPtr nextToken = lexToken(file, position, isCommand, false);
                // Determine if the next token implies a variable (i.e. it is LPAREN, COLON, EQUALS, or an operation
                // token).
                if (nextToken->type == LPAREN || nextToken->type == COLON || nextToken->type == EQUALS
                    || nextToken->type == EXPONENTIATE_EQUALS || nextToken->type == DOUBLE_EQUALS
                    || nextToken->type == NOT_EQUALS || nextToken->type == LESSER_EQUAL
                    || nextToken->type == GREATER_EQUAL || nextToken->type == MODULO_EQUALS
                    || nextToken->type == DIVIDE_EQUALS || nextToken->type == MULTIPLY_EQUALS
                    || nextToken->type == MINUS_EQUALS || nextToken->type == ADD_EQUALS || nextToken->type == LSQUARE
                    || (noSpace && (nextToken->type == DECREMENT || nextToken->type == INCREMENT))) {
                    tokens.push_back(nextToken);
                } else {
                    // If it isn't a variable, it's a command, so reset the position index so that the command string
                    // will be properly lexed.
                    tokens.pop_back();
                    token->type = CMDSTRINGVAL;
                    isCommand = true;
                    commandPosition = token->position;
                    position = token->position;
                }
            }
            // Look ahead for alias
            if (token->type == ALIAS && isFirst) {
                tokens.push_back(expectToken(VARIABLE, file, position, isCommand));
                tokens.push_back(expectToken(EQUALS, file, position, isCommand));
                isCommand = true;
            }
            // Look ahead for timeout
            if (token->type == TIMEOUT && isFirst) {
                tokens.push_back(expectToken(INTVAL, file, position, isCommand));
                isCommand = true;
            }
            // Look ahead for for-loop
            if (token->type == FOR && isFirst && !isCommand) {
                tokens.push_back(expectToken(LPAREN, file, position, isCommand));
                lexStatement(tokens, file, position, SEMICOLON);
                lexExpression(tokens, file, position, SEMICOLON);
                lexStatement(tokens, file, position, RPAREN);
            }
            skipWhitespace(file, position);
            if ((token->type == ALIAS || token->type == TIMEOUT) && isFirst) { commandPosition = position; }
            if (isFirst) { isFirst = false; }
        }
        if (isCommand && isBacktickCommand) {
            throw Util::CommanderException("Command was not terminated with a backtick", commandPosition);
        }
        if (isCommand) {
            throw Util::CommanderException("Command was not terminated with " + tokenTypeToString(terminatingToken)
                                                   + " token",
                                           commandPosition);
        }
        throw Util::CommanderException(
                "Statement was not terminated with " + tokenTypeToString(terminatingToken) + " token", startPosition);
    }

    void lexExpression(TokenList& tokens, const std::string& file, FilePosition& position,
                       const tokenType& terminatingToken) {
        const FilePosition startPosition = position;
        bool isCommand = false;
        FilePosition commandPosition;
        skipWhitespace(file, position);
        while (position.index < file.length()) {
            const TokenPtr token = lexToken(file, position, isCommand, false);
            if (token->type == BACKTICK) {
                if (!isCommand) {
                    commandPosition = token->position;
                    isCommand = true;
                } else {
                    isCommand = false;
                }
            }
            tokens.push_back(token);
            if (token->type == terminatingToken && isCommand) { break; }
            if (token->type == terminatingToken) { return; }
            skipWhitespace(file, position);
        }
        if (isCommand) {
            throw Util::CommanderException("Command was not terminated with a backtick", commandPosition);
        }
        throw Util::CommanderException("Expression not terminated by " + tokenTypeToString(terminatingToken) + " token",
                                       startPosition);
    }

}  // namespace Lexer