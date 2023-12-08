/**
 * @file source/parser/parse_table.cpp
 * @brief Implements the `ParseTable` class.
 * @details See `parse_table.hpp` for documentation.
 */

#include "parse_table.hpp"

#include "source/util/combine_hashes.hpp"

#include <unordered_set>

namespace Parser {

    ParseTable::ParseTable() {
        constexpr ASTNodeType goalNode = ASTNodeType::PRGM;
        const Grammar::GrammarRule defaultRule {goalNode, {goalNode}};

        //  Memory-intensive initialization.
        const Grammar grammar;

        Closure closure(grammar.rules);

        //  An initializer inside an initializer inside an initializer inside an initializer.
        //  This line constructs a vector of states with length 1,
        //  and that state is made up of a vector of kernels of length 1.
        //
        //  If you add optional types for clarity, it would look like this:
        //      `std::vector<State> { State { std::vector<Kernel> { Kernel {...} } } };`
        //  but then you'd be spoiling the fun.
        std::vector<State> states {{{{defaultRule, 0, TokenType::END_OF_FILE}}}};

        //  Initially, `states` has only one state in it.
        //  In most iterations, we'll add to `states` until we get to the point
        //  where we've gone through all the states and there aren't more to add.
        for (StateNum stateNum = 0; stateNum < states.size(); ++stateNum) {
            const State& state = states[stateNum];

            //  Start by calculating this state's closure.
            const auto& clos = closure[state.kernels];

            //  Calculate the transitions for this iteration's state.
            std::unordered_map<GrammarEntry, std::vector<Kernel>, Grammar::GrammarEntry::Hash> transitions;

            //  For all kernels in the enclosure:
            for (const auto& enclosed : clos) {
                //  If the kernel is incomplete (i.e., the index is less than the number of components):
                const auto& components = enclosed.rule.get().components;
                if (enclosed.index < components.size()) {
                    //  Get the next symbol in the kernel's components and add it to the transition map.
                    const auto next = [&]() -> GrammarEntry {
                        if (enclosed.rule.get() == defaultRule) return goalNode;

                        return components[enclosed.index];
                    }();

                    transitions[next].emplace_back(enclosed.rule, enclosed.index + 1, enclosed.lookahead);
                }
            }

            //  For each symbol in the transition table, we need to determine which state we need to transition to
            //  upon encountering that symbol.
            for (const auto& entry : transitions) {
                const auto& symbol = std::get<0>(entry);
                const auto& kernels = std::get<1>(entry);

                //  Is there an existing state in the state vector that matches our list of kernels?
                const auto kernelsMatch = [&](const std::vector<Kernel>& lhs, const std::vector<Kernel>& rhs) -> bool {
                    if (lhs.size() != rhs.size()) return false;

                    for (const auto& lKernel : lhs) {
                        bool same = false;
                        for (const auto& rKernel : rhs) {
                            if (lKernel == rKernel) {
                                same = true;
                                break;
                            }
                        }
                        if (!same) return false;
                    }

                    return true;
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
        //  We just need to populate the `_nextAction` and `nextState` tables.

        for (StateNum stateNum = 0; stateNum < states.size(); ++stateNum) {
            const auto& state = states[stateNum];

            for (const auto& transition : state.transitions) {
                const auto& grammarEntry = std::get<0>(transition);
                const auto& nextState = std::get<1>(transition);

                if (grammarEntry.tokenType.has_value()) {
                    //  Add a `SHIFT` action.
                    _nextAction[stateNum][grammarEntry.tokenType.value()] = {ParserAction::ActionType::SHIFT,
                                                                             nextState};
                } else {
                    //  Set the next state.
                    _nextState[stateNum][grammarEntry.nodeType.value()] = {nextState};
                }
            }

            for (const auto& kernel : state.kernels) {
                const auto& rule = kernel.rule.get();
                if (rule == defaultRule) {
                    //  Add an `ACCEPT` action.
                    _nextAction[stateNum][TokenType::END_OF_FILE] = {ParserAction::ActionType::ACCEPT};
                } else if (kernel.index == rule.components.size()
                           && _nextAction[stateNum].count(kernel.lookahead) == 0) {
                    //  Add a `REDUCE` action.
                    _nextAction[stateNum][kernel.lookahead] = {rule.components.size(), grammar.reductions.at(rule)};
                }
            }
        }
    }

    ParserAction ParseTable::getNextAction(ParseTable::StateNum stateNum, TokenType tokenType) {
        return _nextAction[stateNum][tokenType];
    }

    ParseTable::StateNum ParseTable::getNextState(ParseTable::StateNum stateNum, ASTNodeType nodeType) {
        return _nextState[stateNum][nodeType];
    }

    ParseTable::Kernel::Kernel(const Parser::ParseTable::GrammarRule& rule, size_t index,
                               Parser::ParseTable::TokenType lookahead)
        : rule(rule), index(index), lookahead(lookahead) {}

    bool ParseTable::Kernel::operator==(const ParseTable::Kernel& other) const {
        return (this->rule.get() == other.rule.get() && this->index == other.index
                && this->lookahead == other.lookahead);
    }

    bool ParseTable::Kernel::operator!=(const ParseTable::Kernel& other) const { return !(*this == other); }

    size_t ParseTable::Kernel::Hash::operator()(const ParseTable::Kernel& kernel) const {
        const std::hash<size_t> hash;

        size_t val = Grammar::GrammarRule::Hash {}(kernel.rule);
        val = Util::combineHashes(val, hash(kernel.index));
        val = Util::combineHashes(val, hash(kernel.lookahead));

        return val;
    }

    ParseTable::State::State() = default;

    ParseTable::State::State(const std::vector<Kernel>& kernels,
                             const std::unordered_map<GrammarEntry, StateNum, Grammar::GrammarEntry::Hash>& transitions)
        : kernels(kernels), transitions(transitions) {}

    ParseTable::Closure::Closure(const std::vector<GrammarRule>& grammar) : _grammar(grammar) {}

    std::vector<ParseTable::Kernel> ParseTable::Closure::operator[](const std::vector<ParseTable::Kernel>& kernels) {
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
            std::vector<Kernel>& closure = _closure[kernel];

            if (closure.empty()) {
                std::unordered_set<Kernel, Kernel::Hash> usedKernels;
                closure = {kernel};

                bool finished = false;
                while (!finished) {
                    finished = true;
                    for (size_t ind = 0; ind < closure.size(); ++ind) {
                        const Kernel& curKernel = closure[ind];

                        const std::vector<GrammarEntry> remaining {curKernel.rule.get().components.begin()
                                                                           + (long)curKernel.index,
                                                                   curKernel.rule.get().components.end()};

                        if (remaining.empty()) continue;

                        const std::unordered_set<TokenType> lookaheads = [&]() -> std::unordered_set<TokenType> {
                            if (remaining.size() < 2) return {curKernel.lookahead};
                            return getFirst(remaining[1]);
                        }();

                        for (const auto& rule : _grammar) {
                            if (!remaining[0].nodeType.has_value() || remaining[0].nodeType.value() != rule.result)
                                continue;

                            for (const auto& lookahead : lookaheads) {
                                const Kernel newKernel {rule, 0, lookahead};

                                if (usedKernels.count(newKernel) != 0) continue;

                                usedKernels.insert(newKernel);
                                closure.push_back(newKernel);
                            }
                        }
                    }
                }
            }

            result.insert(_closure[kernel].begin(), _closure[kernel].end());
        }

        return {result.begin(), result.end()};
    }
}  //  namespace Parser
