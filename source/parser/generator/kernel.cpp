/**
 * @file source/parser/generator/kernel.cpp
 * @details Implements the parse table generator's `Kernel` class and methods.
 */

#include "kernel.hpp"

#include "grammar.hpp"

#include "source/parser/ast_node.hpp"
#include "source/util/combine_hashes.hpp"

#include <cstddef>
#include <functional>
#include <ostream>
#include <vector>

namespace Parser {

    Kernel::Kernel() : rule(), index(), lookahead() {}

    Kernel::Kernel(const GrammarRule* const rule, const std::size_t index, const TokenType lookahead)
        : rule(rule), index(index), lookahead(lookahead) {}

    bool Kernel::operator==(const Kernel& other) const {
        constexpr std::hash<Kernel> hash;
        return hash(*this) == hash(other);
    }

    bool Kernel::operator!=(const Kernel& other) const { return !(*this == other); }

    bool Kernel::operator<(const Kernel& other) const {
        if (this->index != other.index) return this->index < other.index;
        if (this->lookahead != other.lookahead) return this->lookahead < other.lookahead;
        return *this->rule < *other.rule;
    }

    std::ostream& operator<<(std::ostream& stream, const Kernel& kernel) {
        stream << "{" << kernel.rule->priority << ": (" << nodeTypeToString(kernel.rule->result) << ") -> ";
        for (std::size_t ind = 0; ind < kernel.rule->components.size(); ++ind) {
            if (ind == kernel.index) stream << "* ";
            stream << kernel.rule->components[ind] << " ";
        }
        if (kernel.index == kernel.rule->components.size()) stream << "* ";
        stream << ":: [" << tokenTypeToString(kernel.lookahead) << "]}";

        return stream;
    }


}  //  namespace Parser

std::size_t std::hash<Parser::Kernel>::operator()(const Parser::Kernel& kernel) const noexcept {
    return Util::combineHashes(kernel.index, kernel.lookahead, *kernel.rule);
}
