/**
 * @file lexer.hpp
 * @brief Contains header content for lexer.cpp
 *
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#ifndef COMMANDER_LEXER_HPP
#define COMMANDER_LEXER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace lexer {

    /**
     * @brief Enumeration of different types of tokens.
     */
    enum tokenType : std::uint8_t {
        ADD,
        ADD_EQUALS,
        ALIAS,
        AMPERSAND,
        AND,
        BACKTICK,
        BOOL,
        BREAK,
        CMDSTRINGVAL,
        COLON,
        COMMA,
        CONST,
        CONTINUE,
        DECREMENT,
        DIVIDE,
        DIVIDE_EQUALS,
        DO,
        DOT,
        DOUBLE_EQUALS,
        ELSE,
        END_OF_FILE,
        EXPONENTIATE,
        EXPONENTIATE_EQUALS,
        EQUALS,
        FALSE,
        FLOAT,
        FLOATVAL,
        FOR,
        GREATER,
        GREATER_EQUAL,
        IF,
        IMPORT,
        INCREMENT,
        INT,
        INTVAL,
        LAMBDA,
        LCURLY,
        LESSER,
        LESSER_EQUAL,
        LPAREN,
        LSQUARE,
        MINUS,
        MINUS_EQUALS,
        MODULO,
        MODULO_EQUALS,
        MULTIPLY,
        MULTIPLY_EQUALS,
        NOT,
        NOT_EQUALS,
        OR,
        PIPE,
        PRINT,
        PRINTLN,
        QUESTION,
        RCURLY,
        READ,
        RETURN,
        RPAREN,
        RSQUARE,
        SCAN,
        SEMICOLON,
        STRING,
        STRINGLITERAL,
        STRINGVAL,
        TO,
        TRUE,
        TYPE,
        UNKNOWN,
        VARIABLE,
        WHILE,
        WRITE
    };

    /**
     * @brief Represents a position in a file
     */
    struct FilePosition {
        /**
         * The file name and path
         */
        std::string fileName;

        /**
         * The line position in the file
         */
        int line;

        /**
         * The column position in the file
         */
        int column;

        /**
         * The index position in the file
         */
        int index;

        /**
         * @brief Returns the string representation of the FilePosition
         * @return The string representation of the FilePosition
         */
        [[nodiscard]] std::string toString() const;
    };

    /**
     * @brief Represents a token from the lexer
     */
    struct Token {
        /**
         * The contents of the token
         */
        std::string contents;

        /**
         * The type of the token
         */
        tokenType type;

        /**
         * The position the token appears in the file
         */
        FilePosition position;

        /**
         * @brief The default constructor for the token
         */
        Token();

        /**
         * @brief Constructor for the token
         * @param contents The string contents of the token
         * @param type The token type
         * @param position The position the token appears in the file
         */
        Token(std::string contents, tokenType type, FilePosition position);

        /**
         * @brief Use default contructor
         */
        virtual ~Token() = default;

        /**
         * @brief Returns the string representation of the Token
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

        /**
         * Sub-tokens of a string, which include multiple string literals and other tokens if there is any string
         * interpolation.
         */
        TokenList subTokens;

        /**
         * @brief Returns the string representation of the Token
         * @return The string representation of the Token
         */
        [[nodiscard]] std::string toString() const override;
    };

    /**
     * Vector of string token literal pairs that are keywords
     */
    const std::vector<std::pair<std::string, tokenType>> keywords(
            {{"alias", ALIAS}, {"bool", BOOL},     {"break", BREAK}, {"const", CONST},   {"continue", CONTINUE},
             {"do", DO},       {"else", ELSE},     {"false", FALSE}, {"float", FLOAT},   {"for", FOR},
             {"if", IF},       {"import", IMPORT}, {"int", INT},     {"print", PRINT},   {"println", PRINTLN},
             {"read", READ},   {"return", RETURN}, {"scan", SCAN},   {"string", STRING}, {"to", TO},
             {"true", TRUE},   {"type", TYPE},     {"while", WHILE}, {"write", WRITE}});

    /**
     * Vector of string token literal pairs that are not keywords (order matters here; longest to shortest)
     */
    const std::vector<std::pair<std::string, tokenType>> tokenLiterals({{"**=", EXPONENTIATE_EQUALS},
                                                                        {"->", LAMBDA},
                                                                        {"==", DOUBLE_EQUALS},
                                                                        {"!=", NOT_EQUALS},
                                                                        {"<=", LESSER_EQUAL},
                                                                        {">=", GREATER_EQUAL},
                                                                        {"&&", AND},
                                                                        {"||", OR},
                                                                        {"**", EXPONENTIATE},
                                                                        {"%=", MODULO_EQUALS},
                                                                        {"/=", DIVIDE_EQUALS},
                                                                        {"*=", MULTIPLY_EQUALS},
                                                                        {"-=", MINUS_EQUALS},
                                                                        {"+=", ADD_EQUALS},
                                                                        {"++", INCREMENT},
                                                                        {"--", DECREMENT},
                                                                        {"+", ADD},
                                                                        {"-", MINUS},
                                                                        {"*", MULTIPLY},
                                                                        {"/", DIVIDE},
                                                                        {"%", MODULO},
                                                                        {">", GREATER},
                                                                        {"<", LESSER},
                                                                        {"!", NOT},
                                                                        {":", COLON},
                                                                        {",", COMMA},
                                                                        {"=", EQUALS},
                                                                        {"{", LCURLY},
                                                                        {"[", LSQUARE},
                                                                        {"?", QUESTION},
                                                                        {"}", RCURLY},
                                                                        {"]", RSQUARE},
                                                                        {".", DOT}});

    /**
     * Vector of string token literal pairs that can be parsed in commands as well as normally
     */
    const std::vector<std::pair<std::string, tokenType>> commandTokenLiterals(
            {{"`", BACKTICK}, {"(", LPAREN}, {")", RPAREN}, {"|", PIPE}, {"&", AMPERSAND}, {";", SEMICOLON}});

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
     * @brief Helper for lexing a literal token, if it exists
     * @param file The file contents being lexed
     * @param position The position in the file
     * @returns The literal token, if it finds one
     */
    TokenPtr
    lexLiteral(const std::string& file, FilePosition& position,
               const std::vector<std::pair<std::basic_string<char>, tokenType>>& literals,
               const std::function<bool(const std::pair<std::basic_string<char>, tokenType>& literal,
                                        const std::string& file, FilePosition& position)>& notLiteralCondition);

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
     * @brief Helper function that checks if a character is a valid first character for a variable
     * @param character The character
     * @return True if the character is a valid first character for a variable
     */
    bool isFirstVariableCharacter(const char& character);

    /**
     * @brief Helper function that checks if a character is a valid variable character
     * @param character The character
     * @return True if the character is a valid variable character
     */
    bool isVariableCharacter(const char& character);

    /**
     * @brief Helper function that checks if a character is a letter
     * @param character The character
     * @return True if the character is a letter
     */
    bool isLetter(const char& character);

    /**
     * @brief Helper function that checks if a character is a digit
     * @param character The character
     * @return True if the character is a digit
     */
    bool isDigit(const char& character);

    /**
     * @brief Helper function that checks if a character is whitespace
     * @param character The character
     * @return True if the character is whitespace
     */
    bool isWhitespace(const char& character);

    /**
     * @brief Helper function that checks if a character is an illegal character
     * @param character The character
     * @return If the character is an illegal character
     */
    bool isIllegalCharacter(const char& character);

    /**
     * @brief Helper function that gets the next token, and ensures its type is what is expected
     * @param type The type of token being expected
     * @param file The file being lexed
     * @param position The current position in the file
     * @param isCommand Tells whether a command is being lexed or not
     * @return The token if the next token is the correct type
     */
    TokenPtr expectToken(const tokenType& type, const std::string& file, FilePosition& position, bool& isCommand);

    /**
     * @brief Helper function that lexes a statement
     * @param tokens The vector that will hold all the tokens
     * @param file The file being lexed
     * @param position The current position in the file
     * @param terminatingToken The token to look for to stop lexing the expression
     */
    void lexStatement(TokenList& tokens, const std::string& file, FilePosition& position,
                      const tokenType& terminatingToken);

    /**
     * @brief Helper function that lexes an expression
     * @param tokens The vector that will hold all the tokens
     * @param file The file being lexed
     * @param position The current position in the file
     * @param terminatingToken The token to look for to stop lexing the expression
     */
    void lexExpression(TokenList& tokens, const std::string& file, FilePosition& position,
                       const tokenType& terminatingToken);

}  // namespace lexer

#endif  // COMMANDER_LEXER_HPP
#pragma clang diagnostic pop