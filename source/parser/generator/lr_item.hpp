/**
 * @file source/parser/generator/lr_item.hpp
 * @brief Defines the LR(0) item, LR(1) item, and LALR(1) item classes.
 */

#ifndef LR_ITEM_HPP
#define LR_ITEM_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"

#include <set>

namespace Parser {

    /**
     * @brief Represents an LR(0) item, consisting of a grammar rule and an index into that rule.
     */
    struct Lr0Item {
        /**
         * @brief Default constructor.
         */
        Lr0Item();

        /**
         * @brief Class constructor.
         *
         * @param rule The rule that this grammar item represents.
         * @param index How far we are into this grammar rule.
         */
        Lr0Item(const GrammarRule* rule, std::size_t index);

        /**
         * @brief Equality operator.
         *
         * @param other The other LR(0) item against which to compare.
         * @return True when these two LR(0) items are equal.
         */
        bool operator==(const Lr0Item& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other LR(0) item against which to compare.
         * @return False when these two LR(0) items are equal.
         */
        bool operator!=(const Lr0Item& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other LR(0) item against which to compare.
         * @return True when these this LR(0) item should be ordered before the other.
         */
        bool operator<(const Lr0Item& other) const;

        /**
         * @brief Streams the given LR(0) item to the given output stream.
         *
         * @param stream The stream to which to output the given LR(0) item.
         * @param lr0Item The LR(0) item to output to the given stream.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const Lr0Item& lr0Item);

        /**
         * @brief A pointer to the grammar rule that this LR(0) item represents.
         * @details Not a smart pointer because this is a non-owning pointer.
         */
        const GrammarRule* rule;

        /**
         * @brief How far we are into this LR(0) item's grammar rule.
         */
        std::size_t index;
    };

    /**
     * @brief Represents an LR(1) item, consisting of a grammar rule, an index into that rule, and a lookahead token.
     */
    struct Lr1Item : Lr0Item {
        /**
         * @brief Default constructor.
         */
        Lr1Item();

        /**
         * @brief Class constructor from an LR(0) item.
         *
         * @param lr0Item The LR(0) item that this LR(1) item extends.
         * @param lookahead The lookahead token of this LR(1) item.
         */
        Lr1Item(const Lr0Item& lr0Item, Lexer::TokenType lookahead);

        /**
         * @brief Class constructor.
         *
         * @param rule The rule that this grammar item represents.
         * @param index How far we are into this grammar rule.
         * @param lookahead The lookahead token of this LR(1) item.
         */
        Lr1Item(const GrammarRule* rule, std::size_t index, Lexer::TokenType lookahead);

        /**
         * @brief Equality operator.
         *
         * @param other The other LR(1) item against which to compare.
         * @return True when these two LR(1) items are equal.
         */
        bool operator==(const Lr1Item& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other LR(1) item against which to compare.
         * @return False when these two LR(1) items are equal.
         */
        bool operator!=(const Lr1Item& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other LR(1) item against which to compare.
         * @return True when these this LR(1) item should be ordered before the other.
         */
        bool operator<(const Lr1Item& other) const;

        /**
         * @brief Streams the given LR(1) item to the given output stream.
         *
         * @param stream The stream to which to output the given LR(1) item.
         * @param lr1Item The LR(1) item to output to the given stream.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const Lr1Item& lr1Item);

        /**
         * @brief The lookahead token of this LR(1) item.
         */
        Lexer::TokenType lookahead;
    };

    /**
     * @brief Represents an LALR(1) item, consisting of a grammar rule, an index into that rule, and lookahead tokens.
     */
    struct LalrItem : Lr0Item {
        /**
         * @brief Default constructor.
         */
        LalrItem();

        /**
         * @brief Class constructor from an LR(0) item.
         *
         * @param lr0Item The LR(0) item that this LALR(1) item extends.
         * @param lookaheads The lookahead tokens of this LALR(1) item.
         */
        LalrItem(const Lr0Item& lr0Item, const std::set<Lexer::TokenType>& lookaheads);

        /**
         * @brief Class constructor.
         *
         * @param rule The rule that this grammar item represents.
         * @param index How far we are into this grammar rule.
         * @param lookaheads The lookahead tokens of this LALR(1) item.
         */
        LalrItem(const GrammarRule* rule, std::size_t index, const std::set<Lexer::TokenType>& lookaheads);

        /**
         * @brief Equality operator.
         *
         * @param other The other LALR(1) item against which to compare.
         * @return True when these two LALR(1) items are equal.
         */
        bool operator==(const LalrItem& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other LALR(1) item against which to compare.
         * @return False when these two LALR(1) items are equal.
         */
        bool operator!=(const LalrItem& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other LALR(1) item against which to compare.
         * @return True when these this LALR(1) item should be ordered before the other.
         */
        bool operator<(const LalrItem& other) const;

        /**
         * @brief Streams the given LALR(1) item to the given output stream.
         *
         * @param stream The stream to which to output the given LALR(1) item.
         * @param lalrItem The LALR(1) item to output to the given stream.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const LalrItem& lalrItem);

        /**
         * @brief The set of lookahead tokens of this LALR(1) item.
         */
        std::set<Lexer::TokenType> lookaheads;
    };

    /**
     * @brief A set of LR(0) items.
     */
    using Lr0ItemSet = std::set<Lr0Item>;
    /**
     * @brief A kernel of LR(0) items.
     */
    using Lr0Kernel = Lr0ItemSet;
    /**
     * @brief A closure of LR(0) items.
     */
    using Lr0Closure = Lr0ItemSet;

    /**
     * @brief A set of LR(1) items.
     */
    using Lr1ItemSet = std::set<Lr1Item>;
    /**
     * @brief A kernel of LR(1) items.
     */
    using Lr1Kernel = Lr1ItemSet;
    /**
     * @brief A closure of LR(1) items.
     */
    using Lr1Closure = Lr1ItemSet;

    /**
     * @brief A set of LALR(1) items.
     */
    using LalrItemSet = std::set<LalrItem>;
    /**
     * @brief A kernel of LALR(1) items.
     */
    using LalrKernel = LalrItemSet;
    /**
     * @brief A closure of LALR(1) items.
     */
    using LalrClosure = LalrItemSet;

    /**
     * @brief Given an LR(0) state and a grammar entry transition, reports the resulting LR(0) kernel.
     *
     * @param lr0State The source LR(0) state.
     * @param nextEntry The grammar item on which to transition.
     * @return The resulting LR(0) kernel.
     */
    Lr0Kernel lr0Goto(const Lr0Closure& lr0State, GrammarEntry nextEntry);

    /**
     * @brief Given an LR(1) state and a grammar entry transition, reports the resulting LR(1) kernel.
     *
     * @param lr1State The source LR(1) state.
     * @param nextEntry The grammar item on which to transition.
     * @return The resulting LR(1) kernel.
     */
    Lr1Kernel lr1Goto(const Lr1Closure& lr1State, GrammarEntry nextEntry);

}  // namespace Parser

namespace std {

    /**
     * @brief Hashing functor for an LR(0) item.
     */
    template<>
    struct hash<Parser::Lr0Item> {
        /**
         * @brief Hashes the given LR(0) item.
         *
         * @param lr0Item The LR(0) item to hash.
         * @return The hash value of the given LR(0) item.
         */
        std::size_t operator()(const Parser::Lr0Item& lr0Item) const noexcept;
    };

    /**
     * @brief Hashing functor for an LR(1) item.
     */
    template<>
    struct hash<Parser::Lr1Item> {
        /**
         * @brief Hashes the given LR(1) item.
         *
         * @param lr1Item The LR(1) item to hash.
         * @return The hash value of the given LR(1) item.
         */
        std::size_t operator()(const Parser::Lr1Item& lr1Item) const noexcept;
    };

    /**
     * @brief Hashing functor for an LALR(1) item.
     */
    template<>
    struct hash<Parser::LalrItem> {
        /**
         * @brief Hashes the given LALR(1) item.
         *
         * @param lalrItem The LALR(1) item to hash.
         * @return The hash value of the given LR(1) item.
         */
        std::size_t operator()(const Parser::LalrItem& lalrItem) const noexcept;
    };

    /**
     * @brief Hashing functor for an LR(0) item set.
     */
    template<>
    struct hash<Parser::Lr0ItemSet> {
        /**
         * @brief Hashes the given LR(0) item set.
         *
         * @param lr0ItemSet The LR(0) item set to hash.
         * @return The hash value of the given LR(0) item set.
         */
        std::size_t operator()(const Parser::Lr0ItemSet& lr0ItemSet) const noexcept;
    };

    /**
     * @brief Hashing functor for an LR(1) item set.
     */
    template<>
    struct hash<Parser::Lr1ItemSet> {
        /**
         * @brief Hashes the given LR(1) item set.
         *
         * @param lr1ItemSet The LR(1) item set to hash.
         * @return The hash value of the given LR(1) item set.
         */
        std::size_t operator()(const Parser::Lr1ItemSet& lr1ItemSet) const noexcept;
    };

    /**
     * @brief Hashing functor for an LALR(1) item set.
     */
    template<>
    struct hash<Parser::LalrItemSet> {
        /**
         * @brief Hashes the given LALR(1) item set.
         *
         * @param lalrItemSet The LALR(1) item set to hash.
         * @return The hash value of the given LALR(1) item set.
         */
        std::size_t operator()(const Parser::LalrItemSet& lalrItemSet) const noexcept;
    };

}  // namespace std

#endif  // LR_ITEM_HPP
