/**
 * @file source/parser/parse_table.cpp
 * @brief Implements the `ParseTable` class.
 * @details Provides transitions from parser state to state.
 *
 */

#include "parse_table.hpp"

namespace Parser {

    ParseTable::ParseTable() {
        // TODO
    }

    ParserAction ParseTable::getNextAction(Parser::ParseTable::StateNum stateNum, TokenType tokenType) {
        return _nextAction[stateNum][tokenType];
    }

    ParseTable::StateNum ParseTable::getNextState(Parser::ParseTable::StateNum stateNum, Parser::ASTNodeType nodeType) {
        return _nextState[stateNum][nodeType];
    }

    ParseTable::GrammarEntry::GrammarEntry(TokenType tokenType) : tokenType(tokenType) {}

    ParseTable::GrammarEntry::GrammarEntry(Parser::ASTNodeType nodeType) : nodeType(nodeType) {}

    bool ParseTable::GrammarEntry::operator==(const Parser::ParseTable::GrammarEntry& other) const {
        return (tokenType.has_value() && other.tokenType.has_value() && tokenType == other.tokenType)
            || (nodeType.has_value() && other.nodeType.has_value() && nodeType == other.nodeType);
    }

    bool ParseTable::GrammarEntry::operator!=(const Parser::ParseTable::GrammarEntry& other) const {
        return !(*this == other);
    }

    size_t ParseTable::HashGrammarEntry::operator()(const Parser::ParseTable::GrammarEntry& entry) const {
        size_t hash = 0;
        if (entry.tokenType.has_value()) {
            hash = _combineHashes(hash, std::hash<int>()(static_cast<int>(*entry.tokenType)));
        }
        if (entry.nodeType.has_value()) {
            hash = _combineHashes(hash, std::hash<int>()(static_cast<int>(*entry.nodeType)));
        }
        return hash;
    }

    bool ParseTable::GrammarRule::operator==(const Parser::ParseTable::GrammarRule& other) const {
        return result == other.result && components == other.components;
    }

    bool ParseTable::GrammarRule::operator!=(const Parser::ParseTable::GrammarRule& other) const {
        return !(*this == other);
    }

    size_t ParseTable::HashGrammarRule::operator()(const Parser::ParseTable::GrammarRule& rule) const {
        size_t hash = 0;
        HashGrammarEntry hashFunction;
        for (const GrammarEntry& entry : rule.components) { hash = _combineHashes(hash, hashFunction(entry)); }
        return _combineHashes(hash, std::hash<int>()(static_cast<int>(rule.result)))
    }

    bool ParseTable::Kernel::operator==(const Parser::ParseTable::Kernel& other) const {
        return lookahead == other.lookahead && index == other.index && rule == other.rule;
    }

    bool ParseTable::Kernel::operator!=(const Parser::ParseTable::Kernel& other) const { return !(*this == other); }

    size_t ParseTable::HashKernel::operator()(const Parser::ParseTable::Kernel& kernel) const {
        size_t hash = 0;
        HashGrammarRule hashFunction;
        hash = _combineHashes(hash, std::hash<int>()(static_cast<int>(kernel.lookahead)));
        hash = _combineHashes(hash, kernel.index);
        return _combineHashes(hash, hashFunction(kernel.rule));
    }

    ParseTable::Closure::Closure(const std::vector<GrammarRule>& grammar) : _grammar(grammar) {}

    std::vector<ParseTable::Kernel> ParseTable::Closure::operator[](const std::vector<Kernel>& kernels) {
        // TODO
    }

    size_t ParseTable::_combineHashes(size_t hash1, size_t hash2) {
        // Implementation inspired by Yakk-Adam Nevraumont's post at
        // https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
        return hash1 ^ hash2 + (sizeof(size_t) >= 8 ? 0x517cc1b727220a95 : 0x9e3779b9) + (hash1 << 6) + (hash1 >> 2);
    }

}  // namespace Parser