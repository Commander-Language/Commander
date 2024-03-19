/**
 * @file source/parser/generator/generator.cpp
 * @brief Implements the parse table `Generator` class and methods.
 */

#include "generator.hpp"
#include "grammar.hpp"
#include "lr_item.hpp"

#include "source/parser/ast_node.hpp"

#include "source/util/generated_map.hpp"
#include "source/util/safe_ptr.hpp"
#include "source/util/thread_queue.hpp"

#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

const std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point lastTime = startTime;
std::chrono::steady_clock::time_point curTime;
const std::vector lengths {-21, 17, 15};
template<typename ObjType>
std::string stringVal(const ObjType thing) {
    std::stringstream stream;
    stream << thing;
    return stream.str();
}
template<typename ObjType>
std::string padString(const ObjType thing, const int width) {
    const std::string val = stringVal(thing);
    if (width < 0) { return val + std::string(std::max(0, -width - static_cast<int>(val.size())), ' '); }
    return std::string(std::max(0, width - static_cast<int>(val.size())), ' ') + val;
}
void printOuterLine() {
    std::size_t total = 4;
    for (std::size_t ind = 0; ind < lengths.size(); ++ind) {
        total += std::abs(lengths[ind]) + 4;
        if (ind + 1 < lengths.size()) ++total;
    }

    std::cout << std::string(total, '=') << "\n";
}
void printInnerLine() {
    std::cout << "||";
    for (std::size_t ind = 0; ind < lengths.size(); ++ind) {
        std::cout << std::string(std::abs(lengths[ind]) + 4, '-');
        if (ind + 1 < lengths.size()) std::cout << "+";
    }
    std::cout << "||\n";
}
template<typename... ObjType>
void printLine(const ObjType... args) {
    std::cout << "||";
    // ReSharper disable once CppDFAUnreadVariable
    // ReSharper disable once CppDFAUnusedValue
    std::size_t ind = 0;

    (
            [&](const auto arg) {
                std::cout << "  " << padString(arg, lengths[ind++]) << "  ";
                if (ind < lengths.size()) std::cout << "|";
            }(args),
            ...);

    std::cout << "||\n";
}
void printTime(const std::string& name) {
    curTime = std::chrono::steady_clock::now();
    printLine(name, std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(),
              std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count());
    lastTime = curTime;
}

namespace Parser {
    Generator::Generator() {
        printOuterLine();
        printLine("Stage", "Elapsed time (ms)", "Total time (ms)");
        printOuterLine();


        // ================
        // ||  0: Setup  ||
        // ================

        // This object contains the entirety of the grammar as a set of `GrammarRule`s.
        // Each `GrammarRule` contains a production that dictates,
        // "given this list of tokens and AST nodes, generate this type of AST node."
        // For example, a rule for addition arithmetic expressions might look like "(EXPR) -> (EXPR) [+] (EXPR)".
        //
        // This is a memory-intensive object. Don't copy.
        const Grammar grammar;

        // The grammar's goal AST node type is `ASTNodeType::PRGM`.
        // This is also the result of the first (highest-priority) rule in the grammar specification.
        const ASTNodeType goalNode = grammar.rules[0].result;
        const GrammarRule goalRule {goalNode, {goalNode}};

        // This is a mapping from an AST node type to a set of all grammar rules that produce that node type.
        const std::unordered_map nodeGenerators {[&] {
            std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>> result;
            for (const auto& rule : grammar.rules) result[rule.result].insert(&rule);
            return result;
        }()};

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

        printTime("0: misc. setup");
        printInnerLine();


        // ===============================
        // ||  1: Construct LR(0) sets  ||
        // ===============================

        const std::unordered_map lr0ItemClosure = _lr0ItemClosure(grammar.rules, &goalRule, nodeGenerators);

        const std::unordered_map lr0States = _generateLr0States(lr0ItemClosure, &goalRule);

        printTime("1: LR(0) sets");
        printInnerLine();


        // =================================================
        // ||  2, 3: Initialize and Propagate lookaheads  ||
        // =================================================

        Util::GeneratedMap lr1ItemClosure {_lr1ItemClosureGenerator(firstSet, nodeGenerators)};

        const std::vector lalrStates = _generateLalrStates(lr0States, lr0ItemClosure, lr1ItemClosure, &goalRule);

        printTime("3: Apply lookaheads");
        printInnerLine();


        // ================================
        // ||  4: Compute table actions  ||
        // ================================

        _numStates = lalrStates.size();

        const auto lalrClosure = [&](const LalrKernel& lalrKernel) {
            Lr1Closure closure;
            for (const auto& lalrItem : lalrKernel) {
                for (const auto lookahead : lalrItem.lookaheads) {
                    const auto itemClosure = lr1ItemClosure[{lalrItem.rule, lalrItem.index, lookahead}];
                    closure.insert(itemClosure.begin(), itemClosure.end());
                }
            }
            return closure;
        };

        if (false) {
            std::vector<Lr0Kernel> lr0StateVec(lr0States.size());
            for (const auto& [kernel, stateNum] : lr0States) { lr0StateVec[stateNum] = kernel; }
            for (std::size_t stateNum = 0; stateNum < lr0States.size(); ++stateNum) {
                std::cout << "================================================================================\n"
                          << "State " << stateNum << ":\n";
                const auto& lr0Kernel = lr0StateVec[stateNum];
                for (const auto& item : lr0Kernel) std::cout << "    - " << item << "\n";
                std::cout << "    ----------------------------------------------------------------------------";
                for (const auto& item : _lr0SetClosure(lr0ItemClosure, lr0Kernel)) {
                    if (lr0Kernel.count(item) == 0) std::cout << "    - " << item << "\n";
                }
                std::cout << "\n\n";
            }
        }

        if (true) {
            for (std::size_t stateNum = 0; stateNum < lr0States.size(); ++stateNum) {
                std::cout << "================================================================================\n"
                          << "State " << stateNum << ":\n";
                for (const auto& item : lalrStates[stateNum]) std::cout << "    - " << item << "\n";
            }
        }

        const auto lr1StateNum = [&](const Lr1Kernel& lr1Kernel) {
            const Lr0Kernel result {lr1Kernel.begin(), lr1Kernel.end()};
            return lr0States.at(result);
        };

        for (std::size_t stateNum = 0; stateNum < lalrStates.size(); ++stateNum) {
            struct ShiftAction {
                Lr1ItemSet lr1Items;
                std::size_t priority {0};
            };
            std::unordered_map<TokenType, ShiftAction> shifts;

            std::unordered_map<TokenType, const GrammarRule*> reductions;

            std::unordered_map<ASTNodeType, Lr1ItemSet> gotoTable;

            const auto& lalrKernel = lalrStates[stateNum];
            const auto lr1Closure = lalrClosure(lalrKernel);

            for (const auto& lr1Item : lr1Closure) {
                if (lr1Item.index < lr1Item.rule->components.size()) {
                    const auto& nextItem = lr1Item.rule->components[lr1Item.index];
                    if (nextItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) {
                        // We need to do a "shift" action.
                        shifts[nextItem.tokenType].lr1Items.emplace(lr1Item.rule, lr1Item.index + 1, lr1Item.lookahead);
                        shifts[nextItem.tokenType].priority = std::min(shifts[nextItem.tokenType].priority,
                                                                       lr1Item.rule->priority);
                    } else {
                        // We need to do a "goto" action.
                        gotoTable[nextItem.nodeType].emplace(lr1Item.rule, lr1Item.index + 1, lr1Item.lookahead);
                    }
                } else {
                    // This LR(1) item is complete; we can do a "reduce" action.
                    if (reductions.count(lr1Item.lookahead) == 0
                        || lr1Item.rule->priority < reductions[lr1Item.lookahead]->priority) {
                        reductions[lr1Item.lookahead] = lr1Item.rule;
                    }
                }
            }

            // After we've gone through all of the LR(1) items in this state,
            // we need to apply the different actions we have and the corresponding new states.

            // First, the "goto" table:
            for (const auto& [nodeType, lr1Items] : gotoTable) _nextState[stateNum][nodeType] = lr1StateNum(lr1Items);

            // Next, the "shift" actions:
            for (const auto& [tokenType, shiftAction] : shifts) {
                _nextAction[stateNum][tokenType] = _pair("ParserAction::ActionType::SHIFT",
                                                         std::to_string(lr1StateNum(shiftAction.lr1Items)));
            }

            // And, finally, the "reduce" actions:
            for (const auto& [tokenType, grammarRule] : reductions) {
                if (*grammarRule == goalRule) {
                    _nextAction[stateNum][tokenType] = "ParserAction::ActionType::ACCEPT";
                } else {
                    _nextAction[stateNum][tokenType] = _join("",
                                                             {"{", std::to_string(grammarRule->components.size()), ", ",
                                                              "[&](const ProductionItemList& productionList) { return ",
                                                              grammar.reductions.at(*grammarRule), "; }}"});
                }
            }
        }

        printTime("4: Compute actions");
        printOuterLine();
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
                                               "}  // namespace Parser",
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

    bool Generator::Propagation::operator==(const Propagation& other) const {
        return this->fromItem == other.fromItem && this->toItem == other.toItem && this->fromState == other.fromState
            && this->toState == other.toState;
    }

    std::unordered_map<Lr0Item, Lr0Closure> Generator::_lr0ItemClosure(
            const std::vector<GrammarRule>& grammarRules, const GrammarRule* goalRule,
            const std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>>& nodeGenerators) {
        std::unordered_map<Lr0Item, Lr0Kernel> closure;

        const auto calcClosure = [&](const Lr0Item& item) {
            Lr0Closure itemClosure {item};

            std::queue<Lr0Item> pending;
            pending.push(item);

            while (!pending.empty()) {
                const Lr0Item curItem = pending.front();
                pending.pop();

                if (curItem.index == curItem.rule->components.size()) continue;

                if (closure.count(curItem) > 0) {
                    itemClosure.insert(closure[curItem].begin(), closure[curItem].end());
                    continue;
                }

                const auto nextEntry = curItem.rule->components[curItem.index];
                if (nextEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;
                for (const auto* generator : nodeGenerators.at(nextEntry.nodeType)) {
                    Lr0Item newItem {generator, 0};
                    if (itemClosure.insert(newItem).second) pending.push(newItem);
                }
            }

            closure[item] = itemClosure;
        };

        const auto calcRule = [&](const GrammarRule* rule) {
            for (std::size_t ind = 0; ind <= rule->components.size(); ++ind) calcClosure({rule, ind});
        };

        calcRule(goalRule);
        for (const auto& rule : grammarRules) calcRule(&rule);

        return closure;
    }

    Lr0Closure Generator::_lr0SetClosure(const std::unordered_map<Lr0Item, Lr0Kernel>& lr0ItemClosure,
                                         const Lr0Kernel& kernel) {
        Lr0Closure closure {kernel};
        for (const auto& item : kernel) {
            const auto& itemClosure = lr0ItemClosure.at(item);
            closure.insert(itemClosure.begin(), itemClosure.end());
        }
        return closure;
    }

    std::unordered_map<Lr0Kernel, std::size_t>
    Generator::_generateLr0States(const std::unordered_map<Lr0Item, Lr0Kernel>& lr0ItemClosure,
                                  const GrammarRule* goalRule) {
        std::unordered_map<Lr0Kernel, std::size_t> states;

        const Lr0Kernel initialState {{goalRule, 0}};
        std::queue<Lr0Kernel> pending;
        pending.push(initialState);
        states[initialState] = 0;

        while (!pending.empty()) {
            const Lr0Kernel curState = std::move(pending.front());
            pending.pop();

            std::unordered_map<GrammarEntry, Lr0Kernel> nextKernels;

            for (const auto& [rule, index] : _lr0SetClosure(lr0ItemClosure, curState)) {
                if (index == rule->components.size()) continue;

                nextKernels[rule->components[index]].emplace(rule, index + 1);
            }

            for (const auto& [entry, nextState] : nextKernels) {
                if (states.count(nextState) == 0) {
                    states[nextState] = states.size();
                    pending.push(nextState);
                }
            }
        }

        return states;
    }

    std::function<Lr1Closure(const Lr1Item&)> Generator::_lr1ItemClosureGenerator(
            const std::unordered_map<ASTNodeType, std::unordered_set<TokenType>>& firstSet,
            const std::unordered_map<ASTNodeType, std::unordered_set<const GrammarRule*>>& nodeGenerators) {
        return [&](const Lr1Item& lr1Item) {
            Lr1Closure closure {lr1Item};
            std::queue<Lr1Item> toExamine;
            toExamine.push(lr1Item);

            while (!toExamine.empty()) {
                const auto curItem = toExamine.front();
                toExamine.pop();

                if (curItem.index == curItem.rule->components.size()) continue;

                const auto nextEntry = curItem.rule->components[curItem.index];
                if (nextEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;

                const auto lookaheads = [&, curItem]() -> std::unordered_set<TokenType> {
                    if (curItem.index + 1 == curItem.rule->components.size()) return {curItem.lookahead};

                    const auto followEntry = curItem.rule->components[curItem.index + 1];
                    if (followEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) return {followEntry.tokenType};

                    return firstSet.at(followEntry.nodeType);
                }();

                if (nodeGenerators.count(nextEntry.nodeType) == 0) continue;
                for (const auto* generator : nodeGenerators.at(nextEntry.nodeType)) {
                    for (const auto nextToken : lookaheads) {
                        const Lr1Item newItem {generator, 0, nextToken};
                        if (closure.insert(newItem).second) toExamine.push(newItem);
                    }
                }
            }
            return closure;
        };
    }

    std::vector<LalrKernel>
    Generator::_generateLalrStates(const std::unordered_map<Lr0Kernel, std::size_t>& lr0States,
                                   const std::unordered_map<Lr0Item, Lr0Closure>& lr0ItemClosure,
                                   Util::GeneratedMap<Lr1Item, Lr1Closure>& lr1ItemClosure,
                                   const GrammarRule* goalRule) {
        std::unordered_map<Lr0Kernel, std::unordered_map<Lr0Item, std::unordered_set<TokenType>>> lookaheads;
        std::mutex lookaheadsGuard;

        for (std::size_t ind = 0; ind <= goalRule->components.size(); ++ind) {
            const Lr0Item goalItem {goalRule, ind};
            lookaheads[{goalItem}][goalItem] = {TokenType::END_OF_FILE};
        }

        std::unordered_set<Propagation, Propagation::Hash> propagations;
        std::mutex propagationsGuard;

        std::mutex lr1ItemClosureGuard;

        const auto processKernel = [&](const Lr0Kernel& kernel) {
            const auto lr0State = _lr0SetClosure(lr0ItemClosure, kernel);
            std::unordered_set<Propagation, Propagation::Hash> propagationsBatch;
            std::unordered_map<Lr0Kernel, std::unordered_map<Lr0Item, std::unordered_set<TokenType>>> lookaheadsBatch;

            for (const auto& item : kernel) {
                if (item.index == item.rule->components.size()) continue;

                constexpr auto specialLookahead = TokenType::END;
                const auto& specialClosure = [&] {
                    const std::lock_guard lock(lr1ItemClosureGuard);
                    return lr1ItemClosure[{item, specialLookahead}];
                }();

                for (const auto& enclosed : specialClosure) {
                    if (enclosed.index == enclosed.rule->components.size()) continue;

                    const auto nextKernel = lr0Goto(lr0State, enclosed.rule->components[enclosed.index]);

                    const Lr0Item nextItem {enclosed.rule, enclosed.index + 1};

                    if (enclosed.lookahead == specialLookahead) {
                        // Propagate lookaheads from `enclosed` to `nextKernel[nextItem]`.
                        propagationsBatch.insert({kernel, item, nextKernel, nextItem});
                    } else {
                        // Add `enclosed.lookahead` to `nextKernel[nextItem]`.
                        lookaheadsBatch[nextKernel][nextItem].insert(enclosed.lookahead);
                    }
                }
            }

            {
                const std::lock_guard lock(lookaheadsGuard);
                for (const auto& [kernel, items] : lookaheadsBatch) {
                    auto& lookaheadsEntry = lookaheads[kernel];
                    for (const auto& [item, lookaheads] : items) {
                        lookaheadsEntry[item].insert(lookaheads.begin(), lookaheads.end());
                    }
                }
            }

            {
                const std::lock_guard lock(propagationsGuard);
                propagations.insert(propagationsBatch.begin(), propagationsBatch.end());
            }
        };

        Util::ThreadQueue jobs(1);
        for (const auto& [lr0Kernel, stateNum] : lr0States) jobs.add(processKernel, lr0Kernel);
        jobs.wait();

        printTime("2: Initial lookaheads");
        printInnerLine();


        // ===============================
        // ||  3: Propagate lookaheads  ||
        // ===============================

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& [fromState, fromItem, toState, toItem] : propagations) {
                const auto fromLookaheads = lookaheads[fromState][fromItem];
                auto& toLookaheads = lookaheads[toState][toItem];
                for (const auto fromLookahead : fromLookaheads) {
                    if (toLookaheads.insert(fromLookahead).second) changed = true;
                }
            }
        }

        std::vector<LalrKernel> states(lr0States.size());
        for (const auto& [lr0Kernel, stateNum] : lr0States) {
            LalrKernel newKernel;
            const auto& kernelLookaheads = lookaheads[lr0Kernel];
            for (const auto& lr0Item : lr0Kernel) {
                const LalrItem newItem {lr0Item,
                                        {kernelLookaheads.at(lr0Item).begin(), kernelLookaheads.at(lr0Item).end()}};
                std::cout << newItem << " :: " << std::hash<LalrItem> {}(newItem) << "\n";
                newKernel.insert(newItem);
            }
            states[stateNum] = newKernel;
        }
        return states;
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
}  // namespace Parser

int main(int, char**) {
    const std::string filename = "parse_table.cpp";

    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Error: failed to open file `" << filename << "` for writing.\n";
        return 1;
    }

    // First, build the parse table.
    const Parser::Generator generator;

    // Then, generate the C++ source file.
    generator.generateSource(file);

    return 0;
}