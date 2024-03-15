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

    LalrItem::LalrItem(const Lr0Item& lr0Item, const std::vector<Lexer::TokenType>& lookaheads)
        : Lr0Item(lr0Item), lookaheads(lookaheads) {}

    LalrItem::LalrItem(const GrammarRule* rule, std::size_t index, const std::vector<Lexer::TokenType>& lookaheads)
        : Lr0Item(rule, index), lookaheads(lookaheads) {}

    bool LalrItem::operator==(const LalrItem& other) const {
        if (this->index != other.index) return false;
        if (this->lookaheads.size() != other.lookaheads.size()) return false;
        for (std::size_t ind = 0; ind < this->lookaheads.size(); ++ind) {
            if (this->lookaheads[ind] != other.lookaheads[ind]) return false;
        }
        return true;
    }

    bool LalrItem::operator!=(const LalrItem& other) const { return !(*this == other); }

    bool LalrItem::operator<(const LalrItem& other) const {
        if (this->index != other.index) return this->index < other.index;
        if (this->lookaheads.size() != other.lookaheads.size()) return this->lookaheads < other.lookaheads;
        for (std::size_t ind = 0; ind < this->lookaheads.size(); ++ind) {
            if (this->lookaheads[ind] != other.lookaheads[ind]) return this->lookaheads[ind] < other.lookaheads[ind];
        }
        return false;
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
        stream << " :: { ";
        if (!lalrItem.lookaheads.empty()) {
            for (std::size_t ind = 0; ind < lalrItem.lookaheads.size(); ++ind) {
                stream << GrammarEntry {lalrItem.lookaheads[ind]} << " ";
            }
        }
        stream << "}}";
        return stream;
    }

}  // namespace Parser

namespace std {

    std::size_t hash<Parser::Lr0Item>::operator()(const Parser::Lr0Item& lr0Item) const noexcept {
        return Util::combineHashes(*lr0Item.rule, lr0Item.index);
    }

    std::size_t hash<Parser::Lr1Item>::operator()(const Parser::Lr1Item& lr1Item) const noexcept {
        return Util::combineHashes(*lr1Item.rule, lr1Item.index, lr1Item.lookahead);
    }

    std::size_t hash<Parser::LalrItem>::operator()(const Parser::LalrItem& lalrItem) const noexcept {
        const std::vector<std::size_t> lookaheads {lalrItem.lookaheads.begin(), lalrItem.lookaheads.end()};
        return Util::combineHashes(*lalrItem.rule, lalrItem.index, Util::combineHashes(lookaheads));
    }

    std::size_t hash<Parser::Lr0ItemSet>::operator()(const Parser::Lr0ItemSet& lr0ItemSet) const noexcept {
        std::vector<std::size_t> hashes(lr0ItemSet.size());
        {
            std::size_t ind = 0;
            Util::ThreadQueue threadQueue;
            for (const auto& lr0Item : lr0ItemSet) {
                std::size_t curInd = ind++;
                threadQueue.add([curInd, lr0Item, &hashes] {
                    constexpr std::hash<Parser::Lr0Item> hash;
                    hashes[curInd] = hash(lr0Item);
                });
            }
        }
        return Util::combineHashes(hashes);
    }

    std::size_t hash<Parser::Lr1ItemSet>::operator()(const Parser::Lr1ItemSet& lr1ItemSet) const noexcept {
        std::vector<std::size_t> hashes(lr1ItemSet.size());
        {
            std::size_t ind = 0;
            Util::ThreadQueue threadQueue;
            for (const auto& lr1Item : lr1ItemSet) {
                std::size_t curInd = ind++;
                threadQueue.add([curInd, lr1Item, &hashes] {
                    constexpr std::hash<Parser::Lr1Item> hash;
                    hashes[curInd] = hash(lr1Item);
                });
            }
        }
        return Util::combineHashes(hashes);
    }

    std::size_t hash<set<Parser::LalrItem>>::operator()(const Parser::LalrItemSet& lalrItemSet) const noexcept {
        std::vector<std::size_t> hashes(lalrItemSet.size());
        {
            std::size_t ind = 0;
            Util::ThreadQueue threadQueue;
            for (const auto& lalrItem : lalrItemSet) {
                std::size_t curInd = ind++;
                threadQueue.add([curInd, lalrItem, &hashes] {
                    constexpr std::hash<Parser::LalrItem> hash;
                    hashes[curInd] = hash(lalrItem);
                });
            }
        }
        return Util::combineHashes(hashes);
    }

}  // namespace std
