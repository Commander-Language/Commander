/**
 * @file lexer.hpp
 * @brief Contains header content for lexer.cpp
 *
 */

#ifndef COMMANDER_LEXER_HPP
#define COMMANDER_LEXER_HPP

#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>


namespace Lexer {

    /**
     * @brief Enumeration of different types of tokens.
     */
    enum TokenType {
        ALIAS,
        AMPERSAND,
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
     * Map of string token literals that are keywords
     */
    const std::unordered_map<std::string, TokenType> KEYWORDS({{"alias", ALIAS},
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
                                                               {"string", STRING},
                                                               {"true", TRUE},
                                                               {"type", TYPE},
                                                               {"while", WHILE}});

    /**
     * Map of string token literals that are not keywords
     */
    const std::unordered_map<std::string, TokenType> TOKEN_LITERALS(
            {{"**=", OP},     {"==", OP},   {"!=", OP},      {"<=", OP},     {">=", OP},    {"&&", OP},
             {"||", OP},      {"**", OP},   {"%=", OP},      {"/=", OP},     {"*=", OP},    {"-=", OP},
             {"+=", OP},      {"++", OP},   {"--", OP},      {"+", OP},      {"-", OP},     {"*", OP},
             {"/", OP},       {"%", OP},    {">", OP},       {"<", OP},      {"!", OP},     {"&", AMPERSAND},
             {":", COLON},    {",", COMMA}, {"=", EQUALS},   {"->", LAMBDA}, {"{", LCURLY}, {"(", LPAREN},
             {"[", LSQUARE},  {"|", PIPE},  {"?", QUESTION}, {"}", RCURLY},  {")", RPAREN}, {"]", RSQUARE},
             {";", SEMICOLON}});

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
        std::string toString() const;
    };

    /**
     * @brief Represents a token from the lexer
     */
    struct Token {
        std::string contents;
        TokenType type;
        FilePosition position;

        Token();
        Token(std::string c, TokenType t, FilePosition p);
        /**
         * Returns the string representation of the Token
         * @return The string representation of the Token
         */
        virtual std::string toString() const;
    };

    using TokenList = std::vector<Token>;

    /**
     * @brief Represents specifically a string token, since strings can have sub tokens due to interpolation
     */
    struct StringToken : Token {
        TokenList subTokens;
        /**
         * Returns the string representation of the Token
         * @return The string representation of the Token
         */
        std::string toString() const override;
    };

    /**
     * @brief Helper that turns TokenType into a string
     * @return The string representation of the TokenType
     */
    std::string tokenTypeToString(const TokenType& type);

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
     * @returns The token, if it finds one
     */
    Token lexToken(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal token, if it finds one
     */
    Token lexTokenLiteral(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an keyword token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The keyword token, if it finds one
     */
    Token lexKeyword(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal float token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal float token, if it finds one
     */
    Token lexFloat(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal int token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal int token, if it finds one
     */
    Token lexInt(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal string token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal string token, if it finds one
     */
    Token lexString(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an VARIABLE token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The VARIABLE token, if it finds one
     */
    Token lexVariable(const std::string& file, FilePosition& position);

    /**
     * @brief Lex a command string token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The command string token, if it finds one
     */
    Token lexCommandString(const std::string& file, FilePosition& position);

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

}  // namespace Lexer

#endif  // COMMANDER_LEXER_HPP