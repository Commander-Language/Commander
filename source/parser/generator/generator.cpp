/**
 * @file source/parser/generator/generator.cpp
 * @details Implements the parse table `Generator` class and methods.
 */

#include "generator.hpp"

#include "source/util/combine_hashes.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace Parser {

    Generator::Generator() {
        constexpr ASTNodeType goalNode = ASTNodeType::PRGM;
        const Grammar::GrammarRule defaultRule {goalNode, {goalNode}};

        //  Memory-intensive initialization.
        const Grammar grammar;

        Closure closure(grammar.rules);
        std::unordered_map<StateNum, std::unordered_map<GrammarEntry, size_t, Grammar::GrammarEntry::Hash>>
                transitionPriorities;

        //  An initializer inside an initializer inside an initializer inside an initializer.
        //  This line constructs a vector of states with length 1,
        //  and that state is made up of a set of kernels of length 1.
        //
        //  If you add optional types for clarity, it would look like this:
        //      `KernelSet { State { std::vector<Kernel> { Kernel {...} } } };`
        //  but then you'd be spoiling the fun.
        std::vector<State> states {{{{defaultRule, 0, 0, TokenType::END_OF_FILE}}}};

        //  Initially, `states` has only one state in it.
        //  In most iterations, we'll add to `states` until we get to the point
        //  where we've gone through all the states and there aren't more to add.
        for (StateNum stateNum = 0; stateNum < states.size(); ++stateNum) {
            const State& state = states[stateNum];

            //  Start by calculating this state's closure.
            const auto& clos = closure[state.kernels];

            //  Calculate the transitions for this iteration's state.
            std::unordered_map<GrammarEntry, KernelSet, Grammar::GrammarEntry::Hash> transitions;

            //  For all kernels in the enclosure:
            for (const auto& enclosed : clos) {
                //  If the kernel is incomplete (i.e., the index is less than the number of components):
                const auto& components = enclosed.rule.get().components;
                if (enclosed.index < components.size()) {
                    //  Get the next symbol in the kernel's components and add it to the transition map.
                    const auto next = [&]() -> GrammarEntry {
                        if (enclosed.priority == 0) return {goalNode};

                        return components[enclosed.index];
                    }();

                    if (transitions[next].empty()) {
                        transitionPriorities[stateNum][next] = enclosed.priority;
                    } else {
                        transitionPriorities[stateNum][next] = std::min(transitionPriorities[stateNum][next],
                                                                        enclosed.priority);
                    }
                    transitions[next].emplace(enclosed.rule, enclosed.priority, enclosed.index + 1, enclosed.lookahead);
                }
            }

            //  For each symbol in the transition table, we need to determine which state we need to transition to
            //  upon encountering that symbol.
            for (const auto& entry : transitions) {
                const auto& symbol = std::get<0>(entry);
                const auto& kernels = std::get<1>(entry);

                //  Is there an existing state in the state vector that matches our list of kernels?
                const auto kernelsMatch = [&](const KernelSet& lhs, const KernelSet& rhs) -> bool {
                    if (lhs.size() != rhs.size()) return false;

                    const auto cmp = [&](const Kernel& kernel) { return rhs.count(kernel) > 0; };
                    return std::all_of(lhs.begin(), lhs.end(), cmp);
                };

                //  Search for a matching state in the state table.
                bool foundMatch = false;
                for (size_t stateInd = 0; stateInd < states.size(); ++stateInd) {
                    if (kernelsMatch(kernels, states[stateInd].kernels)) {
                        states[stateNum].transitions[symbol] = stateInd;
                        foundMatch = true;
                        break;
                    }
                }

                //  If there wasn't a suitable state, then we add a new state to the table.
                if (!foundMatch) {
                    states[stateNum].transitions[symbol] = states.size();
                    states.emplace_back(kernels);
                }
            }
        }

        //  Finally, we have our set of states, complete with definitions of where to go upon encountering which symbol.
        //  We just need to populate the `_nextAction` and `_nextState` tables.
        for (StateNum stateNum = 0; stateNum < states.size(); ++stateNum) {
            const auto& state = states[stateNum];

            for (const auto& transition : state.transitions) {
                const auto& grammarEntry = std::get<0>(transition);
                const auto& nextState = std::get<1>(transition);

                if (grammarEntry.tokenType.has_value()) {
                    //  Add a `SHIFT` action.
                    _nextAction[stateNum][grammarEntry.tokenType.value()] = _pair("ParserAction::ActionType::SHIFT",
                                                                                  nextState);
                } else if (grammarEntry.nodeType.has_value()) {
                    //  Set the next state.
                    _nextState[stateNum][grammarEntry.nodeType.value()] = {nextState};
                }
            }

            for (const auto& kernel : state.kernels) {
                const auto& rule = kernel.rule.get();

                if (rule == defaultRule) {
                    //  Add an `ACCEPT` action.
                    _nextAction[stateNum][TokenType::END_OF_FILE] = _wrap("ParserAction::ActionType::ACCEPT");
                    continue;
                }

                if (kernel.index != rule.components.size()) continue;

                if (_nextAction[stateNum].count(kernel.lookahead) == 0
                    || kernel.priority < transitionPriorities[stateNum][kernel.lookahead]) {
                    //  Add a `REDUCE` action.
                    const std::string function = _join("", {"[&](const ProductionItemList& productionList) { return ",
                                                            grammar.reductions.at(rule), "; }"});
                    _nextAction[stateNum][kernel.lookahead] = _pair(rule.components.size(), function);
                    transitionPriorities[stateNum][kernel.lookahead] = kernel.priority;
                }
            }
        }
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
            "    std::shared_ptr<TypeNode> castType(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<TypeNode>(node);",
            "    }",
            "    std::shared_ptr<VariableNode> castVariable(const std::shared_ptr<ASTNode>& node) {",
            "        return std::reinterpret_pointer_cast<VariableNode>(node);",
            "    }",
            ""
        });

        const std::string foot = _join("\n", {
            "    ParserAction ParseTable::getNextAction(ParseTable::StateNum stateNum, TokenType tokenType) {",
            "        return _nextAction[stateNum][tokenType];",
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

        std::vector<std::string> allNextActions(_nextAction.size());
        for (const auto& statePair : _nextAction) {
            const size_t allActionsIndex = statePair.first;
            std::unordered_map<TokenType, ParserActionInitializer> statesNextActions = statePair.second;
            std::vector<std::string> statesNextActionStrings(statesNextActions.size());
            size_t statesNextActionIndex = 0;
            for (const auto& tokenPair : statesNextActions) {
                TokenType tokenType = tokenPair.first;
                ParserActionInitializer action = tokenPair.second;
                const std::string tokenTypeString = "lexer::tokenType::" + lexer::tokenTypeToString(tokenType);
                statesNextActionStrings[statesNextActionIndex++] = _pair(tokenTypeString, action);
            }
            allNextActions[allActionsIndex] = _wrap(_join(", ", statesNextActionStrings));
        }

        std::vector<std::string> allNextStates(_nextState.size());
        {
            size_t allStatesIndex = 0;
            for (const auto& statePair : _nextState) {
                const StateNum state = statePair.first;
                const std::unordered_map<ASTNodeType, StateNum> statesNextStates = statePair.second;
                std::vector<std::string> statesNextStatesStrings(statesNextStates.size());
                size_t statesNextActionIndex = 0;
                for (const auto& nodePair : statesNextStates) {
                    const ASTNodeType nodeType = nodePair.first;
                    const StateNum nextState = nodePair.second;
                    const std::string tokenTypeString = "ASTNodeType::" + nodeTypeToString(nodeType);
                    statesNextStatesStrings[statesNextActionIndex++] = _pair(tokenTypeString, nextState);
                }
                allNextStates[allStatesIndex++] = _pair(state, _wrap(_join(", ", statesNextStatesStrings)));
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

    Generator::Kernel::Kernel(const Generator::GrammarRule& rule, size_t priority, size_t index,
                              Generator::TokenType lookahead)
        : rule(rule), priority(priority), index(index), lookahead(lookahead) {}

    bool Generator::Kernel::operator==(const Generator::Kernel& other) const {
        Generator::Kernel::Hash hash;
        return (hash(*this) == hash(other));
    }

    bool Generator::Kernel::operator!=(const Generator::Kernel& other) const { return !(*this == other); }

    size_t Generator::Kernel::Hash::operator()(const Generator::Kernel& kernel) const {
        const std::hash<size_t> hash;

        size_t val = Grammar::GrammarRule::Hash {}(kernel.rule);
        val = Util::combineHashes(val, hash(kernel.index));
        val = Util::combineHashes(val, hash(kernel.lookahead));

        return val;
    }

    Generator::State::State() = default;

    Generator::State::State(KernelSet kernels,
                            const std::unordered_map<GrammarEntry, StateNum, Grammar::GrammarEntry::Hash>& transitions)
        : kernels(std::move(kernels)), transitions(transitions) {}

    Generator::Closure::Closure(const std::vector<GrammarRule>& grammar) : _grammar(grammar) {}

    Generator::KernelSet Generator::Closure::operator[](const KernelSet& kernels) {
        //  Given a `GrammarEntry`, returns a set of all possible first token types.
        //  I.e., if the entry is a token type, returns a set of that token type;
        //        if the entry is a node type, returns a set of all possible token types
        //        that can come first in a production that makes that node.
        const auto getFirst = [&](const GrammarEntry& entry) -> std::unordered_set<TokenType> {
            std::unordered_set<ASTNodeType> visited;
            std::unordered_set<TokenType> result;

            const std::function<void(const GrammarEntry&)> getFirstRec = [&](const GrammarEntry& entry) {
                if (entry.tokenType.has_value()) {
                    result.insert(entry.tokenType.value());
                    return;
                }

                const auto nodeType = entry.nodeType.value();
                visited.insert(nodeType);

                for (const auto& rule : _grammar) {
                    if (rule.result != nodeType) continue;

                    const auto& first = rule.components[0];

                    if (first.tokenType.has_value()) {
                        result.insert(first.tokenType.value());
                        continue;
                    }

                    const auto& nodeTypeVal = first.nodeType.value();

                    if (visited.count(nodeTypeVal) == 0) {
                        visited.insert(nodeTypeVal);
                        getFirstRec(first);
                    }
                }
            };
            getFirstRec(entry);
            return result;
        };

        std::unordered_set<Kernel, Kernel::Hash> result;

        for (const auto& kernel : kernels) {
            KernelSet& closureSet = _closure[kernel];

            if (closureSet.empty()) {
                std::vector<Kernel> closureVec {kernel};

                KernelSet usedKernels;

                bool finished = false;
                while (!finished) {
                    finished = true;
                    for (size_t ind = 0; ind < closureVec.size(); ++ind) {
                        const Kernel& curKernel = closureVec[ind];

                        const auto& curRule = curKernel.rule.get();
                        if (curKernel.index == curRule.components.size()) continue;

                        const std::vector<GrammarEntry> remaining {curRule.components.begin() + (long)curKernel.index,
                                                                   curRule.components.end()};

                        const auto lookaheads = [&]() -> std::unordered_set<TokenType> {
                            if (remaining.size() < 2) return {curKernel.lookahead};
                            return getFirst(remaining[1]);
                        }();

                        for (size_t ruleIndex = 0; ruleIndex < _grammar.size(); ++ruleIndex) {
                            const auto& rule = _grammar[ruleIndex];
                            if (!remaining[0].nodeType.has_value()
                                || remaining[0].nodeType.value()  //  NOLINT(*-unchecked-optional-access)
                                           != rule.result)
                                continue;

                            for (const auto& lookahead : lookaheads) {
                                const Kernel newKernel {rule, ruleIndex + 1, 0, lookahead};

                                if (usedKernels.count(newKernel) != 0) continue;

                                usedKernels.insert(newKernel);
                                closureVec.push_back(newKernel);
                            }
                        }
                    }
                }

                closureSet.insert(closureVec.begin(), closureVec.end());
            }

            result.insert(_closure[kernel].begin(), _closure[kernel].end());
        }

        return {result.begin(), result.end()};
    }

    std::string Generator::_join(const std::string& delimiter, const std::vector<std::string>& strings) {
        std::stringstream stream;
        for (const auto& string : strings) stream << string << delimiter;
        const std::string result = stream.str();
        if (!result.empty() && !delimiter.empty()) return result.substr(0, result.size() - delimiter.size());
        return result;
    }

    std::string Generator::_wrap(const std::string& contents) { return "{" + contents + "}"; }

    template<typename LeftType, typename RightType>
    std::string Generator::_pair(const LeftType& left, const RightType& right) {
        std::stringstream leftStream;
        leftStream << left;
        std::stringstream rightStream;
        rightStream << right;
        return _wrap(_join(", ", {leftStream.str(), rightStream.str()}));
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
    Parser::Generator generator;

    //  Then, generate the C++ source file.
    generator.generateSource(file);

    return 0;
}
