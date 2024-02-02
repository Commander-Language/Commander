/**
 * @file source/parser/generator/grammar.hpp
 * @brief Contains the definition of the Commander grammar.
 * @details Well, technically, the definition of the grammar itself is in `grammar.cpp`.
 *     This is the definition of the `Grammar` class.
 */

#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include "source/parser/parser_action.hpp"

#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Parser {

    /**
     * @brief Defines the grammar of the parser.
     */
    class Grammar {
    public:
        /**
         * @brief Shorthand for the `TokenType` enumeration from the lexer.
         */
        using TokenType = lexer::tokenType;

        /**
         * @brief A method that makes a new AST node smart pointer.
         * @details Uses a list of `ProductionItem`s.
         */
        using NodeConstructor = std::string;

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
            TokenType tokenType;

            /**
             * @brief The AST node type of this `GrammarEntry`.
             * @details Void if `tokenType` is set.
             */
            ASTNodeType nodeType;

            /**
             * @brief Class constructor for a token type.
             *
             * @param tokenType The token type of this `GrammarEntry`.
             */
            GrammarEntry(TokenType tokenType);

            /**
             * @brief Class constructor for an AST node type.
             *
             * @param tokenType The AST node type of this `GrammarEntry`.
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
             * @brief Hashing functor for `GrammarEntry` objects.
             */
            struct Hash {
                /**
                 * @brief Hashes the given `GrammarEntry`.
                 *
                 * @param entry The grammar entry to hash.
                 * @return The hash value of the given grammar entry.
                 */
                size_t operator()(const GrammarEntry& entry) const;
            };
        };

        /**
         * @brief Defines a grammar rule for constructing an AST node.
         */
        struct GrammarRule {
            /**
             * @brief The resulting AST node type.
             */
            ASTNodeType result;

            /**
             * @brief The parts that make up the grammar rule.
             */
            std::vector<GrammarEntry> components;

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
             * @brief Hashing functor for `GrammarRule` objects.
             */
            struct Hash {
                /**
                 * @brief Hashes the given `GrammarRule`.
                 *
                 * @param rule The grammar rule to hash.
                 * @return The hash value of the given grammar rule.
                 */
                size_t operator()(const GrammarRule& rule) const;
            };
        };

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
        const std::unordered_map<GrammarRule, NodeConstructor, GrammarRule::Hash> reductions;

    private:
        /**
         * @brief Class constructor with an explicit grammar passed to it.
         * @param grammarDefinitions The grammar definitions from which to build this `Grammar` object.
         */
        Grammar(const std::vector<std::tuple<GrammarRule, NodeConstructor>>& grammarDefinitions);

        /**
         * @brief Returns the grammar definition as a vector of tuples.
         * @details Each tuple is made up of a grammar rule and a callable node constructor function.
         *
         * @return The grammar definition as a vector of tuples.
         */
        static std::vector<std::tuple<GrammarRule, NodeConstructor>> _defineGrammar();
    };

}  //  namespace Parser

#endif  //  GRAMMAR_HPP
