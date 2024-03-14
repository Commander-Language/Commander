#include "lr_item.hpp"

#include "source/util/combine_hashes.hpp"
#include "source/util/thread_queue.hpp"

#include <iostream>

namespace Parser {
    LR0Item::LR0Item() : rule(), index() {}

    LR0Item::LR0Item(const GrammarRule* rule, const std::size_t index) : rule(rule), index(index) {}

    bool LR0Item::operator==(const LR0Item& other) const {
        return this->index == other.index && *this->rule == *other.rule;
    }

    bool LR0Item::operator!=(const LR0Item& other) const { return !(*this == other); }

    bool LR0Item::operator<(const LR0Item& other) const {
        if (this->index != other.index) return this->index < other.index;
        return *this->rule < *other.rule;
    }

    std::ostream& operator<<(std::ostream& stream, const LR0Item& lr0Item) {
        stream << "{" << GrammarEntry {lr0Item.rule->result} << " ->";
        for (std::size_t ind = 0; ind < lr0Item.index; ++ind) { stream << " " << lr0Item.rule->components[ind]; }
        stream << " *";
        for (std::size_t ind = lr0Item.index; ind < lr0Item.rule->components.size(); ++ind) {
            stream << " " << lr0Item.rule->components[ind];
        }
        stream << "}";
        return stream;
    }

    LR1Item::LR1Item() : lookahead() {}

    LR1Item::LR1Item(const LR0Item& lr0Item, const Lexer::TokenType lookahead)
        : LR0Item(lr0Item), lookahead(lookahead) {}

    LR1Item::LR1Item(const GrammarRule* rule, const std::size_t index, const Lexer::TokenType lookahead)
        : LR0Item(rule, index), lookahead(lookahead) {}

    bool LR1Item::operator==(const LR1Item& other) const {
        return this->index == other.index && this->lookahead == other.lookahead && *this->rule == *other.rule;
    }

    bool LR1Item::operator!=(const LR1Item& other) const { return !(*this == other); }

    bool LR1Item::operator<(const LR1Item& other) const {
        if (this->index != other.index) return this->index < other.index;
        if (this->lookahead != other.lookahead) return this->lookahead < other.lookahead;
        return *this->rule < *other.rule;
    }

    std::ostream& operator<<(std::ostream& stream, const LR1Item& lr1Item) {
        stream << "{" << GrammarEntry {lr1Item.rule->result} << " ->";
        for (std::size_t ind = 0; ind < lr1Item.index; ++ind) { stream << " " << lr1Item.rule->components[ind]; }
        stream << " *";
        for (std::size_t ind = lr1Item.index; ind < lr1Item.rule->components.size(); ++ind) {
            stream << " " << lr1Item.rule->components[ind];
        }
        stream << " :: " << GrammarEntry {lr1Item.lookahead} << "}";
        return stream;
    }
}  // namespace Parser

namespace std {
    std::size_t hash<Parser::LR0Item>::operator()(const Parser::LR0Item& lr0Item) const noexcept {
        return Util::combineHashes(*lr0Item.rule, lr0Item.index);
    }

    std::size_t hash<Parser::LR1Item>::operator()(const Parser::LR1Item& lr1Item) const noexcept {
        return Util::combineHashes(*lr1Item.rule, lr1Item.index, lr1Item.lookahead);
    }

    std::size_t hash<Parser::LR0ItemSet>::operator()(const Parser::LR0ItemSet& lr0ItemSet) const noexcept {
        std::vector<std::size_t> hashes(lr0ItemSet.size());
        {
            std::size_t ind = 0;
            Util::ThreadQueue threadQueue;
            for (const auto& lr0Item : lr0ItemSet) {
                std::size_t curInd = ind++;
                threadQueue.add([curInd, lr0Item, &hashes] {
                    constexpr std::hash<Parser::LR0Item> hash;
                    hashes[curInd] = hash(lr0Item);
                });
            }
        }
        return Util::combineHashes(hashes);
    }

    std::size_t hash<Parser::LR1ItemSet>::operator()(const Parser::LR1ItemSet& lr1ItemSet) const noexcept {
        std::vector<std::size_t> hashes(lr1ItemSet.size());
        {
            std::size_t ind = 0;
            Util::ThreadQueue threadQueue;
            for (const auto& lr1Item : lr1ItemSet) {
                std::size_t curInd = ind++;
                threadQueue.add([curInd, lr1Item, &hashes] {
                    constexpr std::hash<Parser::LR1Item> hash;
                    hashes[curInd] = hash(lr1Item);
                });
            }
        }
        return Util::combineHashes(hashes);
    }
}  // namespace std
