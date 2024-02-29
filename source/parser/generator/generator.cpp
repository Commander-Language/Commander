/**
 * @file source/parser/generator/generator.cpp
 * @details Implements the parse table `Generator` class and methods.
 */

#include "generator.hpp"
#include "grammar.hpp"
#include "kernel.hpp"

#include "source/parser/ast_node.hpp"
#include "source/util/combine_hashes.hpp"
#include "source/util/generated_map.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Parser {
    struct PotentialAction {
        enum PotentialActionType : std::uint8_t { SHIFT, REDUCE, ACCEPT };

        PotentialAction() : type(), priority(), shiftNextState(), reduceRule() {}

        PotentialAction(const PotentialActionType type) : type(type), priority(0), shiftNextState(), reduceRule() {}

        PotentialAction(const std::size_t shiftNextState, const std::size_t priority)
            : type(SHIFT), priority(priority), shiftNextState(shiftNextState), reduceRule() {}

        PotentialAction(const GrammarRule* reduceRule, const std::size_t priority)
            : type(REDUCE), priority(priority), shiftNextState(), reduceRule(reduceRule) {}

        friend std::ostream& operator<<(std::ostream& stream, const PotentialAction& action) {
            stream << "{";
            switch (action.type) {
                case SHIFT:
                    stream << "SHIFT: -> " << action.shiftNextState;
                    break;
                case REDUCE:
                    stream << "REDUCE: " << *action.reduceRule;
                    break;
                case ACCEPT:
                    stream << "ACCEPT";
                    break;
            }
            stream << "}";
            return stream;
        }

        PotentialActionType type;

        std::size_t priority;

        std::size_t shiftNextState;

        const GrammarRule* reduceRule;
    };

    Generator::Generator() {
        //  Runs the given operation on each element of the given array in a multithreaded manner.
        const auto multiForEach = [](auto& inputs, const auto& func) {
            constexpr std::size_t numThreads = 8;

            std::vector<std::thread> threads(numThreads);
            for (std::size_t threadNum = 0; threadNum < numThreads; ++threadNum) {
                threads[threadNum] = std::thread([threadNum, numThreads, &inputs, &func] {
                    for (std::size_t ind = threadNum; ind < inputs.size(); ind += numThreads) func(inputs[ind]);
                });
            }
            for (auto& thread : threads) thread.join();
        };

        //  This object contains the entirety of the grammar as a set of `GrammarRule`s.
        //  Each `GrammarRule` contains a production that dictates,
        //  "given this list of tokens and AST nodes, generate this type of AST node."
        //  For example, a rule for addition arithmetic expressions might look like "(EXPR) -> (EXPR) [+] (EXPR)".
        //
        //  This is a memory-intensive object. Don't copy.
        const Grammar grammar;

        //  The grammar's goal AST node type is `ASTNodeType::PRGM`.
        //  This is also the result of the first (highest-priority) rule in the grammar specification.
        const ASTNodeType goalNode = grammar.rules[0].result;
        const GrammarRule goalRule {goalNode, {goalNode}};

        //  A `KernelSet` simply represents an ordered set of kernels.
        using KernelSet = std::set<Kernel>;
        struct HashKernelSet {
            std::size_t operator()(const KernelSet& kernelSet) const {
                constexpr std::hash<Kernel> hash;
                constexpr std::size_t numThreads = 8;

                const std::vector<Kernel> kernels {kernelSet.begin(), kernelSet.end()};
                std::vector<std::size_t> hashes(kernelSet.size());
                std::vector<std::thread> threads(numThreads);
                for (std::size_t threadNum = 0; threadNum < numThreads; ++threadNum) {
                    threads[threadNum] = std::thread([&kernels, threadNum, &hashes, hash] {
                        for (std::size_t kernelInd = threadNum; kernelInd < kernels.size(); kernelInd += numThreads) {
                            hashes[kernelInd] = hash(kernels[kernelInd]);
                        }
                    });
                }
                for (auto& thread : threads) thread.join();

                return Util::combineHashes(hashes);
            }
        };

        //  This is a mapping from an AST node type to a set of all grammar rules that produce that node type.
        const std::unordered_map nodeGenerators {[&] {
            std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>> result;
            for (const auto& rule : grammar.rules) result[rule.result].insert(&rule);
            return result;
        }()};

        //  Reports the set of all token types that can be the first of the given AST node type.
        const std::unordered_map firstSet {[&] {
            std::unordered_map<ASTNodeType, std::unordered_set<TokenType>> result;

            for (const auto& [nodeType, del] : nodeGenerators) {
                std::unordered_set<ASTNodeType> visitedNodeTypes;
                std::unordered_set<TokenType> tokenSet;

                const std::function<void(const ASTNodeType&)> getFirstRec = [&](const ASTNodeType& curNodeType) {
                    visitedNodeTypes.insert(curNodeType);

                    if (nodeGenerators.count(curNodeType) > 0) {
                        for (const auto& rule : nodeGenerators.at(curNodeType)) {
                            const auto& firstItem = rule->components[0];

                            if (firstItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) {
                                tokenSet.insert(firstItem.tokenType);
                                continue;
                            }

                            if (visitedNodeTypes.count(firstItem.nodeType) == 0) {
                                visitedNodeTypes.insert(firstItem.nodeType);
                                getFirstRec(firstItem.nodeType);
                            }
                        }
                    }
                };

                getFirstRec(nodeType);

                result[nodeType] = tokenSet;
            }

            return result;
        }()};

        //  Given a kernel, reports the closure of that kernel.
        //  The closure of a kernel is the set of all kernels that can be "next".
        //
        //  Imagine this is our grammar:
        //      (EXPR) -> [INTVAL]
        //      (EXPR) -> (EXPR) [+] (EXPR)
        //      (EXPR) -> (EXPR) [*] (EXPR)
        //
        //  Now, imagine we're finding the closure for this kernel:
        //      (EXPR) -> (EXPR) [+]  |  (EXPR)  <-- The bar means that we're at index 2 into the grammar rule.
        //
        //  Because the next item in the kernel's grammar rule is (EXPR),
        //  any kernels match that have the index 0 and produce a (EXPR).
        //  I.e., the closure of this kernel is the following:
        //      (EXPR) -> (EXPR) [+]  |  (EXPR)    <-- (Index 2)
        //      (EXPR) ->   |  [INTVAL]            <-- (Index 0)
        //      (EXPR) ->   |  (EXPR) [+] (EXPR)   <-- (Index 0)
        //      (EXPR) ->   |  (EXPR) [*] (EXPR)   <-- (Index 0)
        Util::GeneratedMap<Kernel, KernelSet> singleClosure {[&](const Kernel& kernel) {
            KernelSet usedKernels {kernel};
            std::vector kernelVec {kernel};

            for (std::size_t index = 0; index < kernelVec.size(); ++index) {
                const Kernel& currentKernel = kernelVec[index];
                if (currentKernel.index == currentKernel.rule->components.size()) continue;
                const std::vector<GrammarEntry> remaining {currentKernel.rule->components.begin()
                                                                   + static_cast<long>(currentKernel.index),
                                                           currentKernel.rule->components.end()};
                const GrammarEntry& currentItem = currentKernel.rule->components[currentKernel.index];
                if (currentItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;

                const auto lookaheads = [&]() -> std::unordered_set<TokenType> {
                    if (remaining.size() < 2) return {currentKernel.lookahead};

                    if (remaining[1].grammarEntryType == GrammarEntry::TOKEN_TYPE) return {remaining[1].tokenType};
                    return firstSet.at(remaining[1].nodeType);
                }();

                if (nodeGenerators.count(currentItem.nodeType) > 0) {
                    for (const auto& production : nodeGenerators.at(currentItem.nodeType)) {
                        for (const auto& lookahead : lookaheads) {
                            const Kernel newKernel {production, 0, lookahead};
                            if (usedKernels.count(newKernel) > 0) continue;
                            usedKernels.insert(newKernel);
                            kernelVec.push_back(newKernel);
                        }
                    }
                }
            }

            return usedKernels;
        }};
        std::mutex singleClosureLock;

        //  This is like the `singleClosure` map above, but for a `KernelSet`.
        Util::GeneratedMap<KernelSet, KernelSet, HashKernelSet> closure {[&](const KernelSet& kernelSet) {
            KernelSet enclosed;
            for (const auto& kernel : kernelSet) {
                const std::lock_guard lock(singleClosureLock);
                enclosed.insert(singleClosure[kernel].begin(), singleClosure[kernel].end());
            }
            return enclosed;
        }};

        //  This is a vector of all the states in the parser automaton.
        const Kernel initialKernel = {&goalRule, 0, TokenType::END_OF_FILE};

        std::vector globalCanonStates {singleClosure[initialKernel]};
        std::mutex globalCanonStatesLock;

        //  This is a mapping from a set of kernels to the `StateNum` ID of that state.
        Util::GeneratedMap<KernelSet, std::size_t, HashKernelSet> globalCanonStateNum {[&](const KernelSet& kernelSet) {
            std::lock_guard lock(globalCanonStatesLock);
            globalCanonStates.push_back(kernelSet);
            return globalCanonStates.size() - 1;
        }};

        //  The map of shift and reduce actions from each state.
        std::unordered_map<StateNum, std::unordered_map<TokenType, PotentialAction>> globalCanonAction;
        std::mutex globalCanonActionLock;
        globalCanonAction[0][TokenType::END_OF_FILE] = {PotentialAction::ACCEPT};

        //  The map of goto actions to take from each state.
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> globalCanonNextState;

        //  Now, we iterate through all the states. In most iterations, we'll add more states.
        //  Eventually, though, we'll get to the point where there are no new states to add,
        //  and this loop will terminate.
        for (StateNum stateNum = 0; stateNum < globalCanonStates.size(); ++stateNum) {
            const KernelSet currentState = globalCanonStates[stateNum];

            //  From this state, upon encountering a certain `TokenType`,
            //  we may be able to shift that token onto the stack and move to a new state.
            //  This is a record of all `Kernel`s that we can reach after shifting.
            std::unordered_map<TokenType, KernelSet> shifts;
            //  Record the rule priorities for disambiguation.
            std::unordered_map<TokenType, std::size_t> shiftPriorities;

            //  From this state, upon encountering a certain `TokenType`,
            //  we may be able to reduce the top items on the stack into a new AST node.
            //  This is a record of all `Kernel`s that we can reach after reducing.
            std::unordered_map<TokenType, Kernel> reductions;
            //  Record the rule priorities for disambiguation.
            std::unordered_map<TokenType, std::size_t> reductionPriorities;

            //  From this state, after reducing a certain `ASTNodeType`,
            //  we should move to a different state.
            //  This is a record of all kernels that we can reach upon moving over a new AST node.
            std::unordered_map<ASTNodeType, KernelSet> nextStates;

            //  For all kernels in the closure:
            for (const auto& enclosedKernel : currentState) {
                //  If the kernel is complete (i.e., the index is equal to the number of components),
                //  we're able to perform a `REDUCE` action.
                if (enclosedKernel.index == enclosedKernel.rule->components.size()) {
                    //  Consider rule priorities for disambiguation.
                    if (reductions.count(enclosedKernel.lookahead) == 0
                        || enclosedKernel.rule->priority < reductionPriorities[enclosedKernel.lookahead]) {
                        reductions[enclosedKernel.lookahead] = enclosedKernel;
                        reductionPriorities[enclosedKernel.lookahead] = enclosedKernel.rule->priority;
                    }

                    continue;
                }

                //  Otherwise, examine the next item.
                const auto& nextItem = enclosedKernel.rule->components[enclosedKernel.index];
                const Kernel nextKernel {enclosedKernel.rule, enclosedKernel.index + 1, enclosedKernel.lookahead};
                if (nextItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) {
                    //  If the next item type is a token, we can do a shift action.
                    shifts[nextItem.tokenType].insert(nextKernel);
                    //  Record the rule's priority.
                    if (shiftPriorities.count(nextItem.tokenType) == 0
                        || shiftPriorities[nextItem.tokenType] > nextKernel.rule->priority)
                        shiftPriorities[nextItem.tokenType] = nextKernel.rule->priority;
                } else {
                    //  The next item type is a node; we should do a move action.
                    nextStates[nextItem.nodeType].insert(nextKernel);
                }
            }

            //  At this point, we know the set of kernels reachable for every possible action from this state.
            //  Set the transitions in this `Generator`'s fields.
            const std::vector<std::tuple<TokenType, KernelSet>> shiftsVec {shifts.begin(), shifts.end()};
            multiForEach(shiftsVec, [&](const std::tuple<TokenType, KernelSet>& tuple) {
                //  Make a new `SHIFT` action to the state ID of the next state.
                StateNum nextState = globalCanonStateNum[closure[std::get<1>(tuple)]];
                PotentialAction action {nextState, shiftPriorities[std::get<0>(tuple)]};
                std::lock_guard lock(globalCanonActionLock);
                globalCanonAction[stateNum][std::get<0>(tuple)] = action;
            });

            //  For each node type after doing a reduction action, add that to the `_nextState` table.
            for (const auto& [nodeType, nextKernels] : nextStates) {
                globalCanonNextState[stateNum][nodeType] = globalCanonStateNum[closure[nextKernels]];
            }

            //  Finally, add the reduction actions.
            for (const auto& [tokenType, kernel] : reductions) {
                //  The priority of this reduction.
                const auto& priority = reductionPriorities[tokenType];

                //  If this kernel was actually the goal rule, then accept. This was a successful parse.
                if (*kernel.rule == goalRule) {
                    globalCanonAction[stateNum][tokenType] = {PotentialAction::PotentialActionType::ACCEPT};
                    continue;
                }

                //  Add this reduce action, if it has a higher priority than a conflicting shift action.
                if (globalCanonAction[stateNum].count(tokenType) == 0
                    || priority <= globalCanonAction[stateNum][tokenType].priority) {
                    globalCanonAction[stateNum][tokenType] = {kernel.rule, priority};
                }
            }
        }

        //  At this point, we have the canonical LR(1) parse table built and ready to go.
        //  However, it's too bulky to generate and then compile; we need to trim it down into an LALR parse table.
        struct HashLALRKernelSet {
            [[nodiscard]] std::size_t operator()(const KernelSet& kernelSet) const noexcept {
                KernelSet newKernels;
                for (const auto& kernel : kernelSet) {
                    newKernels.emplace(kernel.rule, kernel.index, TokenType::END_OF_FILE);
                }
                return HashKernelSet {}(newKernels);
            }
        };
        constexpr HashLALRKernelSet hash;
        std::unordered_map<std::size_t, StateNum> lalrStateNums;
        std::unordered_map<StateNum, StateNum> lalrStateTranslation;
        std::unordered_map<StateNum, std::unordered_map<TokenType, PotentialAction>> lalrActions;
        for (StateNum state = 0; state < globalCanonStates.size(); ++state) {
            const std::size_t hashVal = hash(globalCanonStates[state]);
            if (lalrStateNums.count(hashVal) == 0) lalrStateNums[hashVal] = lalrStateNums.size();
            lalrStateTranslation[state] = lalrStateNums[hashVal];
        }
        for (StateNum state = 0; state < globalCanonStates.size(); ++state) {
            const StateNum newState = lalrStateTranslation[state];
            for (const auto& [tokenType, action] : globalCanonAction[state]) {
                if (lalrActions[newState].count(tokenType) == 0
                    || action.priority < lalrActions[newState][tokenType].priority) {
                    lalrActions[newState][tokenType] = action;
                }
            }
        }
        for (const auto& [state, actions] : lalrActions) {
            for (const auto& [tokenType, action] : actions) {
                switch (action.type) {
                    case PotentialAction::SHIFT:
                        _nextAction[state][tokenType] = _pair(
                                "ParserAction::ActionType::SHIFT",
                                std::to_string(lalrStateTranslation[action.shiftNextState]));
                        break;
                    case PotentialAction::REDUCE:
                        _nextAction[state][tokenType] = _join(
                                "", {"{", std::to_string(action.reduceRule->components.size()), ", ",
                                     "[&](const ProductionItemList& productionList) { return ",
                                     grammar.reductions.at(*action.reduceRule), "; }}"});
                        break;
                    case PotentialAction::ACCEPT:
                        _nextAction[state][tokenType] = _wrap("ParserAction::ActionType::ACCEPT");
                        break;
                    default:
                        break;
                }
            }
        }
        for (const auto& [stateNum, map] : globalCanonNextState) {
            for (const auto& [nodeType, nextState] : map) {
                _nextState[lalrStateTranslation[stateNum]][nodeType] = lalrStateTranslation[nextState];
            }
        }
        _numStates = lalrStateNums.size();
    }

    void Generator::generateSource(std::ostream& output) const {
        // clang-format off
        const std::string head = _join("\n", {
            "/**",
            " * @brief Implements the `ParseTable` class.",
            " * @details This file is auto-generated by the parser generator.",
            " *     See `source/parser/parse_table.hpp` for usage.",
            " */",
            "",
            "#include \"source/parser/ast_node.hpp\"",
            "#include \"source/parser/parse_table.hpp\"",
            "",
            "namespace Parser {",
            "",
            "    std::shared_ptr<BindingNode> castBinding(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<BindingNode>(node);",
            "    }",
            "    std::shared_ptr<BindingsNode> castBindings(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<BindingsNode>(node);",
            "    }",
            "    std::shared_ptr<CmdNode> castCmd(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<CmdNode>(node);",
            "    }",
            "    std::shared_ptr<ExprNode> castExpr(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<ExprNode>(node);",
            "    }",
            "    std::shared_ptr<ExprsNode> castExprs(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<ExprsNode>(node);",
            "    }",
            "    std::shared_ptr<StmtNode> castStmt(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<StmtNode>(node);",
            "    }",
            "    std::shared_ptr<StmtsNode> castStmts(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<StmtsNode>(node);",
            "    }",
            "    std::shared_ptr<StringNode> castString(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<StringNode>(node);",
            "    }",
            "    std::shared_ptr<StringExprsNode> castStringExprs(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<StringExprsNode>(node);",
            "    }",
            "    std::shared_ptr<TypeNode> castType(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<TypeNode>(node);",
            "    }",
            "    std::shared_ptr<TypesNode> castTypes(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<TypesNode>(node);",
            "    }",
            "    std::shared_ptr<VariableNode> castVariable(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<VariableNode>(node);",
            "    }",
            ""
        });

        const std::string foot = _join("\n", {
            "    ParserAction ParseTable::getNextAction(ParseTable::StateNum stateNum, TokenType TokenType) {",
            "        return _nextAction[stateNum][TokenType];",
            "    }",
            "",
            "    ParseTable::StateNum ParseTable::getNextState(ParseTable::StateNum stateNum, ASTNodeType nodeType) {",
            "        return _nextState[stateNum][nodeType];",
            "    }",
            "",
            "}  //  namespace Parser",
            ""
        });
        // clang-format on

        std::vector<std::string> allNextActions(_numStates, "{}");
        for (const auto& [allActionsIndex, statesNextActions] : _nextAction) {
            std::vector<std::string> statesNextActionStrings(statesNextActions.size());
            std::size_t statesNextActionIndex = 0;
            for (const auto& [tokenType, action] : statesNextActions) {
                const std::string tokenTypeString = "Lexer::TokenType::" + tokenTypeToString(tokenType);
                statesNextActionStrings[statesNextActionIndex++] = _pair(tokenTypeString, action);
            }
            allNextActions[allActionsIndex] = _wrap(_join(", ", statesNextActionStrings));
        }

        std::vector<std::string> allNextStates(_nextState.size());
        {
            std::size_t allStatesIndex = 0;
            for (const auto& [state, statesNextStates] : _nextState) {
                std::vector<std::string> statesNextStatesStrings(statesNextStates.size());
                std::size_t statesNextActionIndex = 0;
                for (const auto& [nodeType, nextState] : statesNextStates) {
                    const std::string tokenTypeString = "ASTNodeType::" + nodeTypeToString(nodeType);
                    statesNextStatesStrings[statesNextActionIndex++] = _pair(tokenTypeString,
                                                                             std::to_string(nextState));
                }
                allNextStates[allStatesIndex++] = _pair(std::to_string(state),
                                                        _wrap(_join(", ", statesNextStatesStrings)));
            }
        }

        output << head << "\n";

        output << "    ParseTable::ParseTable() :\n"
               << "        _nextAction({\n"
               << "            " << _join(",\n            ", allNextActions) << "}),\n"
               << "        _nextState({\n"
               << "            " << _join(",\n            ", allNextStates) << "})\n"
               << "        {}\n";

        output << foot;
    }

    std::string Generator::_join(const std::string& delimiter, const std::vector<std::string>& strings) {
        if (strings.empty()) return "";
        std::stringstream stream;
        for (const auto& string : std::vector<std::string> {strings.begin(), strings.end() - 1}) {
            stream << string << delimiter;
        }
        stream << strings.back();
        return stream.str();
    }

    std::string Generator::_wrap(const std::string& contents) { return "{" + contents + "}"; }

    std::string Generator::_pair(const std::string& left, const std::string& right) {
        return _wrap(_join(", ", {left, right}));
    }

}  //  namespace Parser

int main(int, char**) {
    const std::string filename = "parse_table.cpp";

    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Error: failed to open file `" << filename << "` for writing.\n";
        return 1;
    }

    //  First, build the parse table.
    const Parser::Generator generator;

    //  Then, generate the C++ source file.
    generator.generateSource(file);

    return 0;
}
