/**
 * @file lexer.hpp
 * @brief Contains header content for lexer.cpp
 *
 */

#ifndef COMMANDER_LEXER_HPP
#define COMMANDER_LEXER_HPP

#include <functional>
#include <memory>
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
        CMDSTRINGVAL,
        CMDVARIABLE,
        COLON,
        COMMA,
        CONST,
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
        STRINGVAL,
        TRUE,
        TYPE,
        VARIABLE,
        WHILE
    };

    /**
     * @brief Helper that turns TokenType into a string
     * @return The string representation of the TokenType
     */
    std::string tokenTypeToString(const TokenType& type);

    /**
     * Map of string token literals that are keywords
     */
    const std::unordered_map<std::string, TokenType> KEYWORD_TOKEN_LITERALS({{"alias", ALIAS},
                                                                             {"bool", BOOL},
                                                                             {"const", CONST},
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
    const std::unordered_map<std::string, TokenType> TOKEN_LITERALS({{"&", AMPERSAND},
                                                                     {":", COLON},
                                                                     {",", COMMA},
                                                                     {"=", EQUALS},
                                                                     {"->", LAMBDA},
                                                                     {"{", LCURLY},
                                                                     {"(", LPAREN},
                                                                     {"[", LSQUARE},
                                                                     {"|", PIPE},
                                                                     {"?", QUESTION},
                                                                     {"}", RCURLY},
                                                                     {")", RPAREN},
                                                                     {"]", RSQUARE},
                                                                     {";", SEMICOLON}});

    /**
     * List of OP token literals
     */
    const std::vector<std::string> OPS {"**="
                                        "==",
                                        "!=",
                                        "<=",
                                        ">=",
                                        "&&",
                                        "||",
                                        "**",
                                        "%=",
                                        "/=",
                                        "*=",
                                        "-=",
                                        "+=",
                                        "++",
                                        "--",
                                        "+",
                                        "-",
                                        "*",
                                        "/",
                                        "%",
                                        ">",
                                        "<",
                                        "!"};

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
        std::string toString() const { return *fileName + ":" + std::to_string(line) + ":" + std::to_string(column); }
    };

    /**
     * @brief Represents a token from the lexer
     */
    struct Token {
        std::string contents;
        TokenType type;
        FilePosition position;

        /**
         * Returns the string representation of the Token
         * @return The string representation of the Token
         */
        std::string toString() const { return tokenTypeToString(type) + " '" + contents + "'"; }
    };

    using TokenList = std::vector<Token>;
    using LexerFunction = std::function<Token(const std::string&, FilePosition&)>;

    /**
     * @brief Takes in a path to a file, and tokenizes its contents.
     * @param tokens The vector that will hold all the tokens.
     * @param filePath The path to the file.
     */
    void tokenize(const TokenList& tokens, const std::string& filePath);

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
    FilePosition skipWhitespace(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an OP token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The OP token, if it finds one
     */
    Token lexOp(const std::string& file, FilePosition& position);

    /**
     * @brief Lex an literal token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal token, if it finds one
     */
    Token lexTokenLiteral(const std::string& file, FilePosition& position);

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
     * @brief Lex a command variable token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The command string token, if it finds one
     */
    Token lexCommandVariable(const std::string& file, FilePosition& position);

    /**
     * @brief Lex a command string token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The command string token, if it finds one
     */
    Token lexCommandString(const std::string& file, FilePosition& position);

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