/**
* @file source/parser/parse_table.hpp
* @brief Defines the `ParseTable` class.
* @details Provides transitions from parser state to state.
*
*/

#ifndef PARSE_TABLE_HPP
#define PARSE_TABLE_HPP

#include "parser_action.hpp"

#include "source/lexer/lexer.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace Parser {

   /**
    * @brief The table of state transitions that the parser follows.
    *
    */
   class ParseTable {
   public:
       /**
        * @brief Each state has a unique integer ID.
        *
        */
       using StateNum = unsigned int;

       /**
        * @brief Class constructor.
        * @details Does a lot of initialization work under the hood. Avoid calling more than once.
        *
        */
       ParseTable();

       /**
        * @brief Looking at the next unparsed token, reports which action to take.
        *
        * @param stateNum The current state of the parser.
        * @param tokenType The type of the next unparsed token.
        * @return The next action for the parser to take.
        */
       ParserAction getNextAction(StateNum stateNum, lexer::tokenType tokenType);

       /**
        * @brief Given the top AST node on the stack, reports the next state to use.
        *
        * @param stateNum The current state of the parser.
        * @param nodeType The AST node type of the top AST node on the stack.
        * @return The next state, to which the parser will transition.
        */
       StateNum getNextState(StateNum stateNum, ASTNodeType nodeType);

   private:
       /**
        * @brief A method that makes a new AST node smart pointer.
        * @details Uses a list of `ProductionItem`s.
        *
        */
       using NodeConstructor = ParserAction::NodeConstructor;

       /**
        * @brief Shorthand for the `TokenType` enumeration from the lexer.
        *
        */
       using TokenType = lexer::tokenType;

       /**
        * @brief A component of a grammar rule.
        * @details Either a token type or an AST node type.
        *
        */
       struct GrammarEntry {
           /**
            * @brief The token type of this `GrammarEntry`.
            * @details Void if `nodeType` is set.
            *
            */
           std::optional<TokenType> tokenType;

           /**
            * @brief The AST node type of this `GrammarEntry`.
            * @details Void if `tokenType` is set.
            *
            */
           std::optional<ASTNodeType> nodeType;

           /**
            * @brief Class constructor for a token type.
            *
            * @param tokenType The token type of this `GrammarEntry`.
            */
           GrammarEntry(TokenType tokenType);

           /**
            * @brief Class constructor for an AST node type.
            *
            * @param tokenType The AST node type of this `GrammarEntry`.
            */
           GrammarEntry(ASTNodeType nodeType);

           /**
            * @brief Equality operator.
            *
            * @param other The other `GrammarEntry` against which to compare.
            * @return True if the two grammar entries are equal; false otherwise.
            */
           bool operator==(const GrammarEntry& other) const;

           /**
            * @brief Inequality operator.
            *
            * @param other The other `GrammarEntry` against which to compare.
            * @return False if the two grammar entries are equal; true otherwise.
            */
           bool operator!=(const GrammarEntry& other) const;
       };

       /**
        * @brief Hashing functor for `GrammarEntry` objects.
        *
        */
       struct HashGrammarEntry {
           /**
            * @brief Hashes the given `GrammarEntry`.
            *
            * @param entry The grammar entry to hash.
            * @return The hash value of the given grammar entry.
            */
           size_t operator()(const GrammarEntry& entry) const;
       };

       /**
        * @brief Defines a grammar rule for constructing an AST node.
        *
        */
       struct GrammarRule {
           /**
            * @brief The resulting AST node type.
            *
            */
           ASTNodeType result;

           /**
            * @brief The parts that make up the grammar rule.
            *
            */
           std::vector<GrammarEntry> components;

           /**
            * @brief Equality operator.
            *
            * @param other The other `GrammarRule` against which to compare.
            * @return True if the two grammar rules are equal; false otherwise.
            */
           bool operator==(const GrammarRule& other) const;

           /**
            * @brief Inequality operator.
            *
            * @param other The other `GrammarRule` against which to compare.
            * @return False if the two grammar rules are equal; true otherwise.
            */
           bool operator!=(const GrammarRule& other) const;
       };

       /**
        * @brief Hashing functor for `GrammarRule` objects.
        *
        */
       struct HashGrammarRule {
           /**
            * @brief Hashes the given `GrammarRule`.
            *
            * @param rule The grammar rule to hash.
            * @return The hash value of the given grammar rule.
            */
           size_t operator()(const GrammarRule& rule) const;
       };

       /**
        * @brief A lookahead kernel for building the parse table.
        * @details Consists of a grammar rule,
        *     how far into the grammar rule we are,
        *     and a lookahead token type.
        *
        */
       struct Kernel {
           /**
            * @brief Equality operator.
            *
            * @param other The other `Kernel` against which to compare.
            * @return True if the two kernels are equal; false otherwise.
            */
           bool operator==(const Kernel& other) const;

           /**
            * @brief Inequality operator.
            *
            * @param other The other `Kernel` against which to compare.
            * @return False if the two kernel are equal; true otherwise.
            */
           bool operator!=(const Kernel& other) const;

           /**
            * @brief The `GrammarRule` to which this kernel refers.
            *
            */
           const GrammarRule& rule;

           /**
            * @brief The index into the grammar rule.
            *
            */
           const size_t index;

           /**
            * @brief The lookahead token.
            *
            */
           const TokenType lookahead;
       };

       /**
        * @brief Hashing functor for `Kernel` objects.
        *
        */
       struct HashKernel {
           /**
            * @brief Hashes the given `Kernel`.
            *
            * @param kernel The kernel to hash.
            * @return The hash value of the given kernel.
            */
           size_t operator()(const Kernel& kernel) const;
       };

       /**
        * @brief Represents a state in the parsing automaton.
        *
        */
       struct State {
           std::vector<Kernel> closure;
       };

       /**
        * @brief Represents the closure of all reachable kernels from any single kernel.
        *
        */
       class Closure {
       public:
           /**
            * @brief Class constructor.
            *
            * @param grammar A reference to the grammar rules.
            */
           Closure(const std::vector<GrammarRule>& grammar);

           /**
            * @brief Gets the closure of all reachable kernels from the given set of kernels.
            * @details Performs a calculation on the first query; saves results for subsequent queries.
            *
            * @param kernels The kernels for which to report the reachable kernels.
            * @return The vector of all possible reachable kernels from the given kernels.
            */
           std::vector<Kernel> operator[](const std::vector<Kernel>& kernels);

       private:
           /**
            * @brief A mapping from one kernel to the set of all reachable kernels from it.
            *
            */
           std::unordered_map<Kernel, std::vector<Kernel>, HashKernel> _closure;

           /**
            * @brief A reference to the grammar definition.
            *
            */
           const std::vector<GrammarRule>& _grammar;
       };

       /**
        * @brief Combines two hashed values in a deterministic way.
        *
        * @param hash1 The first hashed value.
        * @param hash2 The second hashed value.
        * @return A unique combination of the two hashed values.
        */
       static size_t _combineHashes(size_t hash1, size_t hash2);

       /**
        * @brief A mapping of a state number and a token type to a parser action.
        *
        */
       std::unordered_map<StateNum, std::unordered_map<TokenType, ParserAction>> _nextAction;

       /**
        * @brief A mapping of a state number an an AST node type to the parser's next state.
        *
        */
       std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;
   };

}  //  namespace Parser

#endif  //  PARSE_TABLE_HPP
