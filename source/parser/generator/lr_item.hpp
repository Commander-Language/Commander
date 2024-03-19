
#ifndef LR_ITEM_HPP
#define LR_ITEM_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"

#include <set>

namespace Parser {

    struct Lr0Item {
        Lr0Item();

        Lr0Item(const GrammarRule* rule, std::size_t index);

        bool operator==(const Lr0Item& other) const;

        bool operator!=(const Lr0Item& other) const;

        bool operator<(const Lr0Item& other) const;

        friend std::ostream& operator<<(std::ostream& stream, const Lr0Item& lr0Item);

        const GrammarRule* rule;

        std::size_t index;
    };

    struct Lr1Item : Lr0Item {
        Lr1Item();

        Lr1Item(const Lr0Item& lr0Item, Lexer::TokenType lookahead);

        Lr1Item(const GrammarRule* rule, std::size_t index, Lexer::TokenType lookahead);

        bool operator==(const Lr1Item& other) const;

        bool operator!=(const Lr1Item& other) const;

        bool operator<(const Lr1Item& other) const;

        friend std::ostream& operator<<(std::ostream& stream, const Lr1Item& lr1Item);

        Lexer::TokenType lookahead;
    };

    struct LalrItem : Lr0Item {
        LalrItem();

        LalrItem(const Lr0Item& lr0Item, const std::set<Lexer::TokenType>& lookaheads);

        LalrItem(const GrammarRule* rule, std::size_t index, const std::set<Lexer::TokenType>& lookaheads);

        bool operator==(const LalrItem& other) const;

        bool operator!=(const LalrItem& other) const;

        bool operator<(const LalrItem& other) const;

        friend std::ostream& operator<<(std::ostream& stream, const LalrItem& lalrItem);

        std::set<Lexer::TokenType> lookaheads;
    };

    using Lr0ItemSet = std::set<Lr0Item>;
    using Lr0Kernel = Lr0ItemSet;
    using Lr0Closure = Lr0ItemSet;

    using Lr1ItemSet = std::set<Lr1Item>;
    using Lr1Kernel = Lr1ItemSet;
    using Lr1Closure = Lr1ItemSet;

    using LalrItemSet = std::set<LalrItem>;
    using LalrKernel = LalrItemSet;
    using LalrClosure = LalrItemSet;

    Lr0Kernel lr0Goto(const Lr0Closure& lr0State, GrammarEntry nextEntry);

    Lr1Kernel lr1Goto(const Lr1Closure& lr1State, GrammarEntry nextEntry);

}  // namespace Parser

namespace std {

    template<>
    struct hash<Parser::Lr0Item> {
        std::size_t operator()(const Parser::Lr0Item& lr0Item) const noexcept;
    };

    template<>
    struct hash<Parser::Lr1Item> {
        std::size_t operator()(const Parser::Lr1Item& lr1Item) const noexcept;
    };

    template<>
    struct hash<Parser::LalrItem> {
        std::size_t operator()(const Parser::LalrItem& lalrItem) const noexcept;
    };

    template<>
    struct hash<Parser::Lr0ItemSet> {
        std::size_t operator()(const Parser::Lr0ItemSet& lr0ItemSet) const noexcept;
    };

    template<>
    struct hash<Parser::Lr1ItemSet> {
        std::size_t operator()(const Parser::Lr1ItemSet& lr1ItemSet) const noexcept;
    };

    template<>
    struct hash<Parser::LalrItemSet> {
        std::size_t operator()(const Parser::LalrItemSet& lalrItemSet) const noexcept;
    };

}  // namespace std

#endif  // LR_ITEM_HPP
