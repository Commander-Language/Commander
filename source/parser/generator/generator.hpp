/**
 * @file source/parser/generator/generator.hpp
 * @brief Defines the parse table `Generator` class and methods.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "grammar.hpp"
#include "lr_item.hpp"

#include "source/lexer/lexer.hpp"
#include "source/parser/ast_node.hpp"
#include "source/util/combine_hashes.hpp"
#include "source/util/generated_map.hpp"

#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Parser {

    /**
     * @brief The parse table generator class.
     * @details Builds the parse table, and then generates C++ source text.
     */
    class Generator {
    public:
        /**
         * @brief Shorthand for the `TokenType` enumeration from the lexer.
         */
        using TokenType = Lexer::TokenType;

        /**
         * @brief Each state has a unique integer ID.
         */
        using StateNum = unsigned int;

        /**
         * @brief Represents an initializer list for a `ParserAction`.
         * @details Copied to the generated output.
         */
        using ParserActionInitializer = std::string;

        /**
         * @brief Class constructor.
         * @details Does a lot of work under the hood. Avoid calling more than once.
         */
        Generator();

        /**
         * @brief Generates C++ source text for the parse table.
         *
         * @param output The stream to which to output the generated C++ source text.
         */
        void generateSource(std::ostream& output) const;

    private:
        struct Propagation {
            Lr0Kernel fromState;
            Lr0Item fromItem;

            Lr0Kernel toState;
            Lr0Item toItem;

            struct Hash {
                std::size_t operator()(const Propagation& propagation) const noexcept {
                    return Util::combineHashes(propagation.fromState, propagation.fromItem, propagation.toState,
                                               propagation.toItem);
                }
            };

            bool operator==(const Propagation& other) const;
        };

        static std::unordered_map<Lr0Item, Lr0Closure>
        _lr0ItemClosure(const std::vector<GrammarRule>& grammarRules, const GrammarRule* goalRule,
                        const std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>>& nodeGenerators);

        static Lr0Closure _lr0SetClosure(const std::unordered_map<Lr0Item, Lr0Kernel>& lr0ItemClosure,
                                         const Lr0Kernel& kernel);

        static std::unordered_map<Lr0Kernel, std::size_t>
        _generateLr0States(const std::unordered_map<Lr0Item, Lr0Kernel>& lr0ItemClosure, const GrammarRule* goalRule);

        static std::function<Lr1Closure(const Lr1Item&)> _lr1ItemClosureGenerator(
                const std::unordered_map<ASTNodeType, std::unordered_set<TokenType>>& firstSet,
                const std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>>& nodeGenerators);

        static std::unordered_map<LalrKernel, std::size_t>
        _generateLalrStates(const std::unordered_map<Lr0Kernel, std::size_t>& lr0States,
                            const std::unordered_map<Lr0Item, Lr0Closure>& lr0ItemClosure,
                            Util::GeneratedMap<Lr1Item, Lr1Closure>& lr1ItemClosure,
                            const GrammarRule* goalRule);

        /**
         * @brief Joins the given items into a string with the given delimiter.
         *
         * @param delimiter The delimiter of the string.
         * @param strings The items that will be joined together.
         * @return The joined string with the given delimiter.
         */
        static std::string _join(const std::string& delimiter, const std::vector<std::string>& strings);

        /**
         * @brief Wraps the given string in curly braces.
         *
         * @param contents The string to put inside curly braces.
         * @return The string wrapped in curly braces.
         */
        static std::string _wrap(const std::string& contents);

        /**
         * @brief Makes a braced pair of the two given strings.
         *
         * @param left The left string.
         * @param right The right string.
         * @return A braced pair of the two given strings.
         */
        static std::string _pair(const std::string& left, const std::string& right);

        /**
         * @brief A mapping of a state number and a token type to a parser action.
         */
        std::unordered_map<StateNum, std::unordered_map<TokenType, ParserActionInitializer>> _nextAction;

        /**
         * @brief A mapping of a state number an an AST node type to the parser's next state.
         */
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;

        /**
         * @brief The number of states in the parse table.
         */
        std::size_t _numStates;
    };

}  //  namespace Parser

/**
 * @brief Main program entry point.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line argument vector.
 */
int main(int argc, char** argv);

#endif  //  GENERATOR_HPP
