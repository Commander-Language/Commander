/**
 * @file source/parser/generator/kernel.cpp
 * @details Implements the parse table generator's `Kernel` class and methods.
 */

#include "kernel.hpp"

#include "grammar.hpp"

#include "source/util/combine_hashes.hpp"

#include <ostream>
#include <utility>
#include <vector>

namespace Parser {

    Kernel::Kernel() : rule(), priority(), index(), lookahead() {}

    Kernel::Kernel(GrammarRule rule, const std::size_t priority, const std::size_t index, const TokenType lookahead)
        : rule(std::move(rule)), priority(priority), index(index), lookahead(lookahead) {}

    bool Kernel::operator==(const Kernel& other) const {
        constexpr std::hash<Kernel> hash;
        return hash(*this) == hash(other);
    }

    bool Kernel::operator!=(const Kernel& other) const { return !(*this == other); }

    bool Kernel::operator<(const Kernel& other) const {
        if (this->priority != other.priority) return this->priority < other.priority;
        if (this->index != other.index) return this->index < other.index;
        return this->lookahead < other.lookahead;
    }

    std::ostream& operator<<(std::ostream& stream, const Kernel& kernel) {
        stream << "{" << kernel.priority << ": (" << nodeTypeToString(kernel.rule.result) << ") -> ";
        for (std::size_t ind = 0; ind < kernel.rule.components.size(); ++ind) {
            if (ind == kernel.index) stream << "* ";
            stream << kernel.rule.components[ind] << " ";
        }
        if (kernel.index == kernel.rule.components.size()) stream << "* ";
        stream << ":: [" << tokenTypeToString(kernel.lookahead) << "]}";

        return stream;
    }


}  //  namespace Parser

namespace std {

    std::size_t hash<Parser::Kernel>::operator()(const Parser::Kernel& kernel) const {
        return Util::combineHashes(kernel.priority, kernel.index, kernel.lookahead, kernel.rule);
    }

}  //  namespace std
