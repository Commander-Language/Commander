/**
 * @file source/parser/generator/lr_item.cpp
 * @brief Implements the LR(0) item, LR(1) item, and LALR(1) item class functions.
 */

#include "lr_item.hpp"

#include "source/util/combine_hashes.hpp"
#include "source/util/thread_queue.hpp"

#include <iostream>

namespace Parser {

    Lr0Item::Lr0Item() : rule(), index() {}

    Lr0Item::Lr0Item(const GrammarRule* rule, const std::size_t index) : rule(rule), index(index) {}

    bool Lr0Item::operator==(const Lr0Item& other) const {
        return this->index == other.index && *this->rule == *other.rule;
    }

    bool Lr0Item::operator!=(const Lr0Item& other) const { return !(*this == other); }

    bool Lr0Item::operator<(const Lr0Item& other) const {
        if (this->index != other.index) return this->index < other.index;
        return *this->rule < *other.rule;
    }

    std::ostream& operator<<(std::ostream& stream, const Lr0Item& lr0Item) {
        stream << "{" << GrammarEntry {lr0Item.rule->result} << " ->";
        for (std::size_t ind = 0; ind < lr0Item.index; ++ind) { stream << " " << lr0Item.rule->components[ind]; }
        stream << " *";
        for (std::size_t ind = lr0Item.index; ind < lr0Item.rule->components.size(); ++ind) {
            stream << " " << lr0Item.rule->components[ind];
        }
        stream << "}";
        return stream;
    }

    Lr1Item::Lr1Item() : lookahead() {}

    Lr1Item::Lr1Item(const Lr0Item& lr0Item, const Lexer::TokenType lookahead)
        : Lr0Item(lr0Item), lookahead(lookahead) {}

    Lr1Item::Lr1Item(const GrammarRule* rule, const std::size_t index, const Lexer::TokenType lookahead)
        : Lr0Item(rule, index), lookahead(lookahead) {}

    bool Lr1Item::operator==(const Lr1Item& other) const {
        return this->index == other.index && this->lookahead == other.lookahead && *this->rule == *other.rule;
    }

    bool Lr1Item::operator!=(const Lr1Item& other) const { return !(*this == other); }

    bool Lr1Item::operator<(const Lr1Item& other) const {
        if (this->index != other.index) return this->index < other.index;
        if (this->lookahead != other.lookahead) return this->lookahead < other.lookahead;
        return *this->rule < *other.rule;
    }

    std::ostream& operator<<(std::ostream& stream, const Lr1Item& lr1Item) {
        stream << "{" << GrammarEntry {lr1Item.rule->result} << " ->";
        for (std::size_t ind = 0; ind < lr1Item.index; ++ind) { stream << " " << lr1Item.rule->components[ind]; }
        stream << " *";
        for (std::size_t ind = lr1Item.index; ind < lr1Item.rule->components.size(); ++ind) {
            stream << " " << lr1Item.rule->components[ind];
        }
        stream << " :: " << GrammarEntry {lr1Item.lookahead} << "}";
        return stream;
    }

    LalrItem::LalrItem() = default;

    LalrItem::LalrItem(const Lr0Item& lr0Item, const std::set<Lexer::TokenType>& lookaheads)
        : Lr0Item(lr0Item), lookaheads(lookaheads) {}

    LalrItem::LalrItem(const GrammarRule* rule, const std::size_t index, const std::set<Lexer::TokenType>& lookaheads)
        : Lr0Item(rule, index), lookaheads(lookaheads) {}

    bool LalrItem::operator==(const LalrItem& other) const {
        if (this->index != other.index) return false;
        if (*this->rule != *other.rule) return false;
        if (this->lookaheads.size() != other.lookaheads.size()) return false;
        if (this->lookaheads.empty()) return true;

        auto thisIter = this->lookaheads.begin();
        auto otherIter = other.lookaheads.begin();
        while (true) {
            if (thisIter == this->lookaheads.end()) return true;
            if (*thisIter != *otherIter) return false;
            ++thisIter;
            ++otherIter;
        }
    }

    bool LalrItem::operator!=(const LalrItem& other) const { return !(*this == other); }

    bool LalrItem::operator<(const LalrItem& other) const {
        if (this->index != other.index) return this->index < other.index;
        if (*this->rule != *other.rule) return *this->rule < *other.rule;
        if (this->lookaheads.size() != other.lookaheads.size()) {
            return this->lookaheads.size() < other.lookaheads.size();
        }
        if (this->lookaheads.empty()) return false;

        auto thisIter = this->lookaheads.begin();
        auto otherIter = other.lookaheads.begin();
        while (true) {
            if (thisIter == this->lookaheads.end()) return false;
            if (*thisIter != *otherIter) return *thisIter < *otherIter;
            ++thisIter;
            ++otherIter;
        }
    }

    Lr0Kernel lr0Goto(const Lr0Closure& lr0State, const GrammarEntry nextEntry) {
        Lr0Kernel newKernel;

        for (const auto& lr0Item : lr0State) {
            if (lr0Item.index == lr0Item.rule->components.size()) continue;

            if (lr0Item.rule->components[lr0Item.index] == nextEntry) {
                newKernel.emplace(lr0Item.rule, lr0Item.index + 1);
            }
        }

        return newKernel;
    }

    Lr1Kernel lr1Goto(const Lr1Closure& lr1State, const GrammarEntry nextEntry) {
        Lr1Kernel newKernel;

        for (const auto& lr1Item : lr1State) {
            if (lr1Item.index == lr1Item.rule->components.size()) continue;

            if (lr1Item.rule->components[lr1Item.index] == nextEntry) {
                newKernel.emplace(lr1Item.rule, lr1Item.index + 1, lr1Item.lookahead);
            }
        }

        return newKernel;
    }

    std::ostream& operator<<(std::ostream& stream, const LalrItem& lalrItem) {
        stream << "{" << GrammarEntry {lalrItem.rule->result} << " ->";
        for (std::size_t ind = 0; ind < lalrItem.index; ++ind) { stream << " " << lalrItem.rule->components[ind]; }
        stream << " *";
        for (std::size_t ind = lalrItem.index; ind < lalrItem.rule->components.size(); ++ind) {
            stream << " " << lalrItem.rule->components[ind];
        }
        stream << " :: {";
        if (!lalrItem.lookaheads.empty()) {
            for (auto iter = lalrItem.lookaheads.begin(); iter != lalrItem.lookaheads.end();) {
                stream << GrammarEntry {*iter};
                if (++iter != lalrItem.lookaheads.end()) stream << " ";
            }
        }
        stream << "}}";
        return stream;
    }

}  // namespace Parser
