/**
 * @file source/parser/generator/grammar.hpp
 * @brief Contains the definition of the Commander grammar.
 * @details Well, technically, the definition of the grammar itself is in `grammar.cpp`.
 *     This is the definition of the `Grammar` class.
 */

#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include "source/lexer/lexer.hpp"
#include "source/parser/ast_node.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Parser {

    /**
     * @brief A component of a grammar rule.
     * @details Either a token type or an AST node type.
     */
    struct GrammarEntry {
        /**
         * @brief An enumeration of the types of `GrammarEntry`s (i.e., a token type or a node type).
         */
        enum GrammarEntryType : uint8_t { TOKEN_TYPE, NODE_TYPE };

        /**
         * @brief The type of this `GrammarEntry`.
         */
        GrammarEntryType grammarEntryType;

        /**
         * @brief The token type of this `GrammarEntry`.
         * @details Void if `nodeType` is set.
         */
        Lexer::TokenType tokenType;

        /**
         * @brief The AST node type of this `GrammarEntry`.
         * @details Void if `TokenType` is set.
         */
        ASTNodeType nodeType;

        /**
         * @brief Class constructor for a token type.
         *
         * @param tokenType The token type of this `GrammarEntry`.
         */
        GrammarEntry(Lexer::TokenType tokenType);

        /**
         * @brief Class constructor for an AST node type.
         *
         * @param nodeType The AST node type of this `GrammarEntry`.
         */
        GrammarEntry(ASTNodeType nodeType);

        /**
         * @brief Equality operator.
         *
         * @param other The other `GrammarEntry` against which to compare.
         * @return True if the two grammar entries are equal; false otherwise.
         */
        bool operator==(const GrammarEntry& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other `GrammarEntry` against which to compare.
         * @return False if the two grammar entries are equal; true otherwise.
         */
        bool operator!=(const GrammarEntry& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other `GrammarEntry` against which to compare.
         * @return True if this grammar entry should come before the other; false otherwise.
         */
        bool operator<(const GrammarEntry& other) const;

        /**
         * @brief Stream insertion operator.
         *
         * @param stream The `std::ostream` that will receive a grammar entry as input.
         * @param grammarEntry The grammar entry to stream to the given `std::ostream`.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const GrammarEntry& grammarEntry);
    };

    /**
     * @brief Defines a grammar rule for constructing an AST node.
     */
    struct GrammarRule {
        /**
         * @brief Whether a grammar rule is left-associative or right-associative.
         */
        enum Associativity : std::uint8_t {
            LEFT_ASSOCIATIVE,
            RIGHT_ASSOCIATIVE,
        };

        /**
         * @brief Default constructor.
         */
        GrammarRule();

        /**
         * @brief Class constructor.
         *
         * @param result The result of applying this grammar rule.
         * @param components The components that make up this grammar rule.
         * @param associativity The associativity of this rule (left or right). Defaults to left.
         * @param priority The priority of this grammar rule. Defaults to 0.
         */
        GrammarRule(ASTNodeType result, const std::vector<GrammarEntry>& components,
                    Associativity associativity = LEFT_ASSOCIATIVE, std::size_t priority = 0);

        /**
         * @brief Equality operator.
         *
         * @param other The other `GrammarRule` against which to compare.
         * @return True if the two grammar rules are equal; false otherwise.
         */
        bool operator==(const GrammarRule& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other `GrammarRule` against which to compare.
         * @return False if the two grammar rules are equal; true otherwise.
         */
        bool operator!=(const GrammarRule& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other `GrammarRule` against which to compare.
         * @return True if this rule should come before the other grammar rule; false otherwise.
         */
        bool operator<(const GrammarRule& other) const;

        /**
         * @brief Stream insertion operator.
         *
         * @param stream The `std::ostream` that will receive a grammar rule as input.
         * @param grammarRule The grammar rule to stream to the given `std::ostream`.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const GrammarRule& grammarRule);

        /**
         * @brief The resulting AST node type.
         */
        ASTNodeType result;

        /**
         * @brief The parts that make up the grammar rule.
         */
        std::vector<GrammarEntry> components;

        /**
         * @brief The priority of this grammar rule.
         */
        std::size_t priority;

        /**
         * @brief The associativity of this rule (left or right).
         */
        Associativity associativity;
    };

}  // namespace Parser

namespace std {

    /**
     * @brief Specialization of `std::hash` for `GrammarEntry` objects.
     */
    template<>
    struct hash<Parser::GrammarEntry> {
        /**
         * @brief Hashes the given `GrammarEntry`.
         *
         * @param grammarEntry The grammar entry to hash.
         * @return The hash value of the given grammar entry.
         */
        size_t operator()(const Parser::GrammarEntry& grammarEntry) const noexcept;
    };

    /**
     * @brief Specialization of `std::hash` for `GrammarRule` objects.
     */
    template<>
    struct hash<Parser::GrammarRule> {
        /**
         * @brief Hashes the given `GrammarRule`.
         *
         * @param grammarRule The grammar rule to hash.
         * @return The hash value of the given grammar rule.
         */
        size_t operator()(const Parser::GrammarRule& grammarRule) const noexcept;
    };

}  // namespace std

namespace Parser {

    /**
     * @brief Defines the grammar of the parser.
     */
    class Grammar {
    public:
        /**
         * @brief Shorthand for the `TokenType` enumeration from the lexer.
         */
        using TokenType = Lexer::TokenType;

        /**
         * @brief A method that makes a new AST node smart pointer.
         * @details Uses a list of `ProductionItem`s.
         */
        using NodeConstructor = std::string;

        /**
         * @brief Class constructor.
         * @details Does a lot of work under the hood. Avoid calling more than once.
         */
        Grammar();

        /**
         * @brief Copy constructor.
         * @details Explicitly deleted.
         */
        Grammar(const Grammar& other) = delete;

        /**
         * @brief The grammar rules to use.
         * @details Ordered by precedence.
         */
        const std::vector<GrammarRule> rules;

        /**
         * @brief The node constructor functions to use.
         */
        const std::unordered_map<GrammarRule, NodeConstructor> reductions;

    private:
        /**
         * @brief Class constructor with an explicit grammar passed to it.
         * @param grammarDefinitions The grammar definitions from which to build this `Grammar` object.
         */
        explicit Grammar(const std::vector<std::tuple<GrammarRule, NodeConstructor>>& grammarDefinitions);

        /**
         * @brief Returns the grammar definition as a vector of tuples.
         * @details Each tuple is made up of a grammar rule and a callable node constructor function.
         *
         * @return The grammar definition as a vector of tuples.
         */
        static std::vector<std::tuple<GrammarRule, NodeConstructor>> _defineGrammar();
    };

}  // namespace Parser

#endif  // GRAMMAR_HPP
