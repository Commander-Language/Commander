/**
 * @file source/parser/generator/kernel.hpp
 * @details Defines the parse table generator's `Kernel` class and methods.
 */

#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "grammar.hpp"

#include <cstddef>
#include <functional>
#include <ostream>

namespace Parser {

    /**
     * @brief A lookahead kernel for building the parse table.
     * @details Consists of a grammar rule, how far into the grammar rule we are, and a lookahead token type.
     */
    class Kernel {
    public:
        /**
         * @brief An enumeration of the different types of tokens.
         */
        using TokenType = Lexer::TokenType;

        /**
         * @brief Default constructor.
         * @details Necessary for use in some containers.
         */
        Kernel();

        /**
         * @brief Class constructor.
         *
         * @param rule The `GrammarRule` to which this kernel refers.
         * @param priority The priority of the `GrammarRule`.
         * @param index How far we are into the `GrammarRule`.
         * @param lookahead The next lookahead token type.
         */
        Kernel(const GrammarRule* rule, std::size_t priority, std::size_t index, TokenType lookahead);

        /**
         * @brief Equality operator.
         *
         * @param other The other `Kernel` against which to compare.
         * @return True if the two `Kernel`s are equal; false otherwise.
         */
        bool operator==(const Kernel& other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other The other `Kernel` against which to compare.
         * @return False if the two `Kernel`s are equal; true otherwise.
         */
        bool operator!=(const Kernel& other) const;

        /**
         * @brief Less-than operator.
         *
         * @param other The other `Kernel` against which to compare.
         * @return True if this `Kernel`'s hashed value is below the other `Kernel`'s hashed value; false otherwise.
         */
        bool operator<(const Kernel& other) const;

        /**
         * @brief Stream insertion operator.
         *
         * @param stream The `std::ostream` that will receive a kernel as input.
         * @param kernel The kernel to stream to the given `std::ostream`.
         * @return The given stream.
         */
        friend std::ostream& operator<<(std::ostream& stream, const Kernel& kernel);

        /**
         * @brief The `GrammarRule` to which this kernel refers.
         * @details Not a smart pointer because the kernel does not own the rule.
         */
        const GrammarRule* rule;

        /**
         * @brief The priority of the grammar rule.
         */
        std::size_t priority;

        /**
         * @brief The index into the grammar rule.
         */
        std::size_t index;

        /**
         * @brief The lookahead token.
         */
        TokenType lookahead;
    };

}  //  namespace Parser

/**
 * @brief Specialization of the `std::hash` class for a `Kernel`.
 */
template<>
struct std::hash<Parser::Kernel> {
    /**
     * @brief Hashing functor call (`()`) operator.
     *
     * @param kernel The kernel to hash.
     * @return The hashed value of the kernel.
     */
    std::size_t operator()(const Parser::Kernel& kernel) const noexcept;
};

#endif  //  KERNEL_HPP
