
#ifndef LR_ITEM_HPP
#define LR_ITEM_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"

#include <set>

namespace Parser {

    struct LR0Item {
        LR0Item();

        LR0Item(const GrammarRule* rule, std::size_t index);

        bool operator==(const LR0Item& other) const;

        bool operator!=(const LR0Item& other) const;

        bool operator<(const LR0Item& other) const;

        friend std::ostream& operator<<(std::ostream& stream, const LR0Item& lr0Item);

        const GrammarRule* rule;

        std::size_t index;
    };

    struct LR1Item : LR0Item {
        LR1Item();

        LR1Item(const LR0Item& lr0Item, Lexer::TokenType lookahead);

        LR1Item(const GrammarRule* rule, std::size_t index, Lexer::TokenType lookahead);

        bool operator==(const LR1Item& other) const;

        bool operator!=(const LR1Item& other) const;

        bool operator<(const LR1Item& other) const;

        friend std::ostream& operator<<(std::ostream& stream, const LR1Item& lr1Item);

        Lexer::TokenType lookahead;
    };

    using LR0ItemSet = std::set<LR0Item>;

    using LR1ItemSet = std::set<LR1Item>;

}  // namespace Parser

namespace std {

    template<>
    struct hash<Parser::LR0Item> {
        std::size_t operator()(const Parser::LR0Item& lr0Item) const noexcept;
    };

    template<>
    struct hash<Parser::LR1Item> {
        std::size_t operator()(const Parser::LR1Item& lr1Item) const noexcept;
    };

    template<>
    struct hash<Parser::LR0ItemSet> {
        std::size_t operator()(const Parser::LR0ItemSet& lr0ItemSet) const noexcept;
    };

    template<>
    struct hash<Parser::LR1ItemSet> {
        std::size_t operator()(const Parser::LR1ItemSet& lr1ItemSet) const noexcept;
    };

}  // namespace std

#endif  // LR_ITEM_HPP
