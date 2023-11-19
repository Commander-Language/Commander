#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
/**
 * @file lexer.hpp
 * @brief Contains header content for lexer.cpp
 *
 */

#ifndef COMMANDER_LEXER_HPP
#define COMMANDER_LEXER_HPP

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace lexer {

    /**
     * @brief Enumeration of different types of tokens.
     */
    enum tokenType : std::uint8_t {
        ALIAS,
        AMPERSAND,
        BACKTICK,
        BOOL,
        BREAK,
        CMDSTRINGVAL,
        COLON,
        COMMA,
        CONST,
        CONTINUE,
        DO,
        ELSE,
        END_OF_FILE,
        EQUALS,
        FALSE,
        FLOAT,
        FLOATVAL,
        FOR,
        IF,
        IMPORT,
        INT,
        INTVAL,
        LAMBDA,
        LCURLY,
        LPAREN,
        LSQUARE,
        OP,
        PIPE,
        QUESTION,
        RCURLY,
        RETURN,
        RPAREN,
        RSQUARE,
        SEMICOLON,
        STRING,
        STRINGLITERAL,
        STRINGVAL,
        TRUE,
        TYPE,
        UNKNOWN,
        VARIABLE,
        WHILE
    };

    /**
     * @brief Represents a position in a file
     */
    struct FilePosition {
        std::shared_ptr<std::string> fileName;
        int line;
        int column;
        int index;

        /**
         * Returns the string representation of the FilePosition
         * @return The string representation of the FilePosition
         */
        [[nodiscard]] std::string toString() const;
    };

    /**
     * @brief Represents a token from the lexer
     */
    struct Token {
        std::string contents;
        tokenType type;
        FilePosition position;

        Token();
        Token(std::string contents, tokenType type, FilePosition position);
        virtual ~Token() = default;
        /**
         * Returns the string representation of the Token
         * @return The string representation of the Token
         */
        [[nodiscard]] virtual std::string toString() const;
    };

    using TokenPtr = std::shared_ptr<Token>;
    using TokenList = std::vector<TokenPtr>;

    /**
     * @brief Represents specifically a string token, since strings can have sub tokens due to interpolation
     */
    struct StringToken : public Token {
        using Token::Token;
        TokenList subTokens;
        /**
         * Returns the string representation of the Token
         * @return The string representation of the Token
         */
        [[nodiscard]] std::string toString() const override;
    };

    /**
     * Map of string token literals that are keywords
     */
    const std::unordered_map<std::string, tokenType> keywords({{"alias", ALIAS},
                                                               {"bool", BOOL},
                                                               {"break", BREAK},
                                                               {"const", CONST},
                                                               {"continue", CONTINUE},
                                                               {"do", DO},
                                                               {"else", ELSE},
                                                               {"false", FALSE},
                                                               {"float", FLOAT},
                                                               {"for", FOR},
                                                               {"if", IF},
                                                               {"import", IMPORT},
                                                               {"int", INT},
                                                               {"return", RETURN},
                                                               {"string", STRING},
                                                               {"true", TRUE},
                                                               {"type", TYPE},
                                                               {"while", WHILE}});

    /**
     * Map of string token literals that are not keywords (using vector data structure since iteration order matters)
     */
    const std::vector<std::pair<std::string, tokenType>> tokenLiterals(
            {{"**=", OP},   {"->", LAMBDA}, {"==", OP},      {"!=", OP},    {"<=", OP},    {">=", OP},    {"&&", OP},
             {"||", OP},    {"**", OP},     {"%=", OP},      {"/=", OP},    {"*=", OP},    {"-=", OP},    {"+=", OP},
             {"++", OP},    {"--", OP},     {"+", OP},       {"-", OP},     {"*", OP},     {"/", OP},     {"%", OP},
             {">", OP},     {"<", OP},      {"!", OP},       {":", COLON},  {",", COMMA},  {"=", EQUALS}, {"{", LCURLY},
             {"(", LPAREN}, {"[", LSQUARE}, {"?", QUESTION}, {"}", RCURLY}, {")", RPAREN}, {"]", RSQUARE}});

    /**
     * Map of string token literals specifically for commands
     */
    const std::unordered_map<std::string, tokenType>
            commandTokenLiterals({{"`", BACKTICK}, {"|", PIPE}, {"&", AMPERSAND}, {";", SEMICOLON}});

    /**
     * @brief Helper that turns TokenType into a string
     * @return The string representation of the TokenType
     */
    std::string tokenTypeToString(const tokenType& type);

    /**
     * @brief Takes in a path to a file, and tokenizes its contents.
     * @param tokens The vector that will hold all the tokens.
     * @param filePath The path to the file.
     */
    void tokenize(TokenList& tokens, const std::string& filePath);

    /**
     * @brief Helper that reads in a file given a path and returns it as a string.
     * @param filePath Path to the file
     * @return The string representing the file contents
     */
    std::string readFile(const std::string& filePath);

    /**
     * @brief Skips whitespace
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The new file position
     */
    void skipWhitespace(const std::string& file, FilePosition& position);

    /**
     * @brief Lex a token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @param isCommand Tells whether a command is being lexed or not
     * @param isFirst Tells whether the token is the first token of a statement or not
     * @returns The token, if it finds one
     */
    TokenPtr lexToken(const std::string& file, FilePosition& position, bool& isCommand, const bool& isFirst);

    /**
     * @brief Lex a literal token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal token, if it finds one
     */
    TokenPtr lexTokenLiteral(const std::string& file, FilePosition& position);

    /**
     * @brief Lex a literal token specifically for commands, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal token for the command, if it finds one
     */
    TokenPtr lexCommandTokenLiteral(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an keyword token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The keyword token, if it finds one
     */
    TokenPtr lexKeyword(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal float token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal float token, if it finds one
     */
    TokenPtr lexFloat(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal int token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal int token, if it finds one
     */
    TokenPtr lexInt(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal string token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal string token, if it finds one
     */
    TokenPtr lexString(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an VARIABLE token specifically for commands (i.e. starts with $), if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The VARIABLE token, if it finds one
     */
    TokenPtr lexCommandVariable(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an VARIABLE token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The VARIABLE token, if it finds one
     */
    TokenPtr lexVariable(const std::string& file, FilePosition& position);

    /**
     * @brief Lex a command string token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The command string token, if it finds one
     */
    TokenPtr lexCommandString(const std::string& file, FilePosition& position);

    /**
     * Helper function that checks if a character is a valid first character for a variable
     * @param character The character
     * @return True if the character is a valid first character for a variable
     */
    bool isFirstVariableCharacter(const char& character);

    /**
     * Helper function that checks if a character is a valid variable character
     * @param character The character
     * @return True if the character is a valid variable character
     */
    bool isVariableCharacter(const char& character);

    /**
     * Helper function that checks if a character is a letter
     * @param character The character
     * @return True if the character is a letter
     */
    bool isLetter(const char& character);

    /**
     * Helper function that checks if a character is a digit
     * @param character The character
     * @return True if the character is a digit
     */
    bool isDigit(const char& character);

    /**
     * Helper function that checks if a character is whitespace
     * @param character The character
     * @return True if the character is whitespace
     */
    bool isWhitespace(const char& character);

    /**
     * Helper function that checks if a character is an illegal character
     * @param character The character
     * @return If the character is an illegal character
     */
    bool isIllegalCharacter(const char& character);

}  // namespace lexer

#endif  // COMMANDER_LEXER_HPP
#pragma clang diagnostic pop