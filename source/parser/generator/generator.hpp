/**
 * @file source/parser/generator/generator.hpp
 * @brief Defines the parse table `Generator` class and methods.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"
#include "source/parser/parser_action.hpp"

#include <ostream>
#include <set>
#include <string>
#include <unordered_map>

namespace Parser {
    //  TODO: Remove.
    std::ostream& operator<<(std::ostream& stream, const lexer::tokenType& tokenType) {
        stream << "[";
        switch (tokenType) {
            case lexer::ADD:
                stream << "ADD";
                break;
            case lexer::ADD_EQUALS:
                stream << "ADD_EQUALS";
                break;
            case lexer::ALIAS:
                stream << "ALIAS";
                break;
            case lexer::AMPERSAND:
                stream << "AMPERSAND";
                break;
            case lexer::AND:
                stream << "AND";
                break;
            case lexer::BACKTICK:
                stream << "BACKTICK";
                break;
            case lexer::BOOL:
                stream << "BOOL";
                break;
            case lexer::BREAK:
                stream << "BREAK";
                break;
            case lexer::CMDSTRINGVAL:
                stream << "CMDSTRINGVAL";
                break;
            case lexer::COLON:
                stream << "COLON";
                break;
            case lexer::COMMA:
                stream << "COMMA";
                break;
            case lexer::CONST:
                stream << "CONST";
                break;
            case lexer::CONTINUE:
                stream << "CONTINUE";
                break;
            case lexer::DECREMENT:
                stream << "DECREMENT";
                break;
            case lexer::DIVIDE:
                stream << "DIVIDE";
                break;
            case lexer::DIVIDE_EQUALS:
                stream << "DIVIDE_EQUALS";
                break;
            case lexer::DO:
                stream << "DO";
                break;
            case lexer::DOUBLE_EQUALS:
                stream << "DOUBLE_EQUALS";
                break;
            case lexer::ELSE:
                stream << "ELSE";
                break;
            case lexer::END_OF_FILE:
                stream << "END_OF_FILE";
                break;
            case lexer::EXPONENTIATE:
                stream << "EXPONENTIATE";
                break;
            case lexer::EXPONENTIATE_EQUALS:
                stream << "EXPONENTIATE_EQUALS";
                break;
            case lexer::EQUALS:
                stream << "EQUALS";
                break;
            case lexer::FALSE:
                stream << "FALSE";
                break;
            case lexer::FLOAT:
                stream << "FLOAT";
                break;
            case lexer::FLOATVAL:
                stream << "FLOATVAL";
                break;
            case lexer::FOR:
                stream << "FOR";
                break;
            case lexer::GREATER:
                stream << "GREATER";
                break;
            case lexer::GREATER_EQUAL:
                stream << "GREATER_EQUAL";
                break;
            case lexer::IF:
                stream << "IF";
                break;
            case lexer::IMPORT:
                stream << "IMPORT";
                break;
            case lexer::INCREMENT:
                stream << "INCREMENT";
                break;
            case lexer::INT:
                stream << "INT";
                break;
            case lexer::INTVAL:
                stream << "INTVAL";
                break;
            case lexer::LAMBDA:
                stream << "LAMBDA";
                break;
            case lexer::LCURLY:
                stream << "LCURLY";
                break;
            case lexer::LESSER:
                stream << "LESSER";
                break;
            case lexer::LESSER_EQUAL:
                stream << "LESSER_EQUAL";
                break;
            case lexer::LPAREN:
                stream << "LPAREN";
                break;
            case lexer::LSQUARE:
                stream << "LSQUARE";
                break;
            case lexer::MINUS:
                stream << "MINUS";
                break;
            case lexer::MINUS_EQUALS:
                stream << "MINUS_EQUALS";
                break;
            case lexer::MODULO:
                stream << "MODULO";
                break;
            case lexer::MODULO_EQUALS:
                stream << "MODULO_EQUALS";
                break;
            case lexer::MULTIPLY:
                stream << "MULTIPLY";
                break;
            case lexer::MULTIPLY_EQUALS:
                stream << "MULTIPLY_EQUALS";
                break;
            case lexer::NOT:
                stream << "NOT";
                break;
            case lexer::NOT_EQUALS:
                stream << "NOT_EQUALS";
                break;
            case lexer::OR:
                stream << "OR";
                break;
            case lexer::PIPE:
                stream << "PIPE";
                break;
            case lexer::PRINT:
                stream << "PRINT";
                break;
            case lexer::PRINTLN:
                stream << "PRINTLN";
                break;
            case lexer::QUESTION:
                stream << "QUESTION";
                break;
            case lexer::RCURLY:
                stream << "RCURLY";
                break;
            case lexer::READ:
                stream << "READ";
                break;
            case lexer::RETURN:
                stream << "RETURN";
                break;
            case lexer::RPAREN:
                stream << "RPAREN";
                break;
            case lexer::RSQUARE:
                stream << "RSQUARE";
                break;
            case lexer::SCAN:
                stream << "SCAN";
                break;
            case lexer::SEMICOLON:
                stream << "SEMICOLON";
                break;
            case lexer::STRING:
                stream << "STRING";
                break;
            case lexer::STRINGLITERAL:
                stream << "STRINGLITERAL";
                break;
            case lexer::STRINGVAL:
                stream << "STRINGVAL";
                break;
            case lexer::TO:
                stream << "TO";
                break;
            case lexer::TRUE:
                stream << "TRUE";
                break;
            case lexer::TYPE:
                stream << "TYPE";
                break;
            case lexer::UNKNOWN:
                stream << "UNKNOWN";
                break;
            case lexer::VARIABLE:
                stream << "VARIABLE";
                break;
            case lexer::WHILE:
                stream << "WHILE";
                break;
            case lexer::WRITE:
                stream << "WRITE";
                break;
        }
        stream << "]";
        return stream;
    }

    //  TODO: Remove.
    std::ostream& operator<<(std::ostream& stream, const Parser::ASTNodeType& nodeType) {
        stream << "(";
        switch (nodeType) {
            case BINDING:
                stream << "BINDING";
                break;
            case BINDINGS:
                stream << "BINDINGS";
                break;
            case CMD:
                stream << "CMD";
                break;
            case EXPR:
                stream << "EXPR";
                break;
            case EXPRS:
                stream << "EXPRS";
                break;
            case PRGM:
                stream << "PRGM";
                break;
            case STMT:
                stream << "STMT";
                break;
            case STMTS:
                stream << "STMTS";
                break;
            case STRING:
                stream << "STRING";
                break;
            case TYPE:
                stream << "TYPE";
                break;
            case VARIABLE:
                stream << "VARIABLE";
                break;
        }
        stream << ")";
        return stream;
    }

    //  TODO: Remove.
    template<typename ItemType>
    std::ostream& operator<<(std::ostream& stream, const std::vector<ItemType>& items) {
        if (!items.empty()) {
            for (const auto& item : std::vector<ItemType> {items.begin(), items.end() - 1}) stream << item << " ";
            stream << items.back();
        }

        return stream;
    }

    //  TODO: Remove.
    std::ostream& operator<<(std::ostream& stream, const Grammar::GrammarEntry& entry) {
        if (entry.grammarEntryType == Grammar::GrammarEntry::TOKEN_TYPE) {
            stream << entry.tokenType;
        } else {
            stream << entry.nodeType;
        }
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const Grammar::GrammarRule& rule) {
        stream << "{" << rule.result << " -> " << rule.components << "}";
        return stream;
    }

    /**
     * @brief The parse table generator class.
     * @details Builds the parse table, and then generates C++ source text.
     */
    class Generator {
    public:
        /**
         * @brief Shorthand for the `TokenType` enumeration from the lexer.
         */
        using TokenType = lexer::tokenType;

        /**
         * @brief Each state has a unique integer ID.
         */
        using StateNum = unsigned int;

        /**
         * @brief Represents an entry in a grammar rule.
         * @details Either a token type or an AST node type.
         *     Every rule is made up of these.
         */
        using GrammarEntry = Grammar::GrammarEntry;

        /**
         * @brief Represents a single grammar rule.
         * @details Made up of the resulting AST node type and a list of component `GrammarEntry`s.
         */
        using GrammarRule = Grammar::GrammarRule;

        /**
         * @brief Represents an initializer list for a `ParserAction`.
         * @details Copied to the generated output.
         */
        using ParserActionInitializer = std::string;

        /**
         * @brief Class constructor.
         * @details Does a lot of work under the hood. Avoid calling more than once.
         */
        Generator();

        /**
         * @brief Generates C++ source text for the parse table.
         *
         * @param output The stream to which to output the generated C++ source text.
         */
        void generateSource(std::ostream& output) const;

    private:
        /**
         * @brief A lookahead kernel for building the parse table.
         * @details Consists of a grammar rule,
         *     how far into the grammar rule we are,
         *     and a lookahead token type.
         */
        struct Kernel {
            /**
             * @brief Hashing functor for `Kernel` objects.
             */
            struct Hash {
                /**
                 * @brief Hashes the given `Kernel`.
                 *
                 * @param kernel The kernel to hash.
                 * @return The hash value of the given kernel.
                 */
                size_t operator()(const Kernel& kernel) const;
            };

            /**
             * @brief Default constructor.
             * @details Necessary for use in some containers.
             */
            Kernel();

            /**
             * @brief Class constructor.
             *
             * @param rule The `GrammarRule` to which this kernel refers.
             * @param priority The priority of the `GrammarRule`.
             * @param index How far we are into the `GrammarRule`.
             * @param lookahead The next lookahead token type.
             */
            Kernel(GrammarRule rule, size_t priority, size_t index, TokenType lookahead);

            /**
             * @brief Equality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return True if the two `Kernel`s are equal; false otherwise.
             */
            bool operator==(const Kernel& other) const;

            /**
             * @brief Inequality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return False if the two `Kernel`s are equal; true otherwise.
             */
            bool operator!=(const Kernel& other) const;

            /**
             * @brief Less-than operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return True if this `Kernel`'s hashed value is below the other `Kernel`'s hashed value; false otherwise.
             */
            bool operator<(const Kernel& other) const;

            /**
             * @brief The `GrammarRule` to which this kernel refers.
             */
            GrammarRule rule;

            /**
             * @brief The priority of the grammar rule.
             */
            size_t priority;

            /**
             * @brief The index into the grammar rule.
             */
            size_t index;

            /**
             * @brief The lookahead token.
             */
            TokenType lookahead;

            //  TODO: Remove.
            friend std::ostream& operator<<(std::ostream& stream, const Kernel& kernel) {
                stream << "<" << kernel.priority << ": " << kernel.rule.result << " -> "
                       << std::vector<GrammarEntry> {kernel.rule.components.begin(),
                                                     kernel.rule.components.begin() + (long)kernel.index}
                       << " * "
                       << std::vector<GrammarEntry> {kernel.rule.components.begin() + (long)kernel.index,
                                                     kernel.rule.components.end()}
                       << " :: " << kernel.lookahead << ">";
                return stream;
            }
        };

        /**
         * @brief A set of `Kernel`s.
         */
        using KernelSet = std::set<Kernel>;

        /**
         * @brief Hashing functor for `KernelSet`s.
         */
        struct HashKernelSet {
            size_t operator()(const KernelSet& kernelSet) const;
        };

        /**
         * @brief Joins the given items into a string with the given delimiter.
         *
         * @param delimiter The delimiter of the string.
         * @param strings The items that will be joined together.
         * @return The joined string with the given delimiter.
         */
        static std::string _join(const std::string& delimiter, const std::vector<std::string>& strings);

        /**
         * @brief Wraps the given string in curly braces.
         *
         * @param contents The string to put inside curly braces.
         * @return The string wrapped in curly braces.
         */
        static std::string _wrap(const std::string& contents);

        /**
         * @brief Makes a braced pair of the two given items.
         *
         * @tparam LeftType The type of the item on the left.
         * @tparam RightType The type of the item on the right.
         * @param left The left item.
         * @param right The right item.
         * @return A braced pair of the two given items.
         */
        template<typename LeftType, typename RightType>
        static std::string _pair(const LeftType& left, const RightType& right);

        /**
         * @brief A mapping of a state number and a token type to a parser action.
         */
        std::unordered_map<StateNum, std::unordered_map<TokenType, ParserActionInitializer>> _nextAction;

        /**
         * @brief A mapping of a state number an an AST node type to the parser's next state.
         */
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;
    };

}  //  namespace Parser

/**
 * @brief Main program entry point.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line argument vector.
 */
int main(int argc, char** argv);

#endif  //  GENERATOR_HPP
