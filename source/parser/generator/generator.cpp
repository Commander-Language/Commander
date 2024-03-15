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
#include <source/util/combine_hashes.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Parser {
    Generator::Generator() {
        const std::chrono::steady_clock::time_point _startTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point _lastTime = _startTime;
        std::chrono::steady_clock::time_point _curTime;
        const std::vector _lengths {-21, 17, 15};
        const auto stringVal = [](const auto thing) {
            std::stringstream stream;
            stream << thing;
            return stream.str();
        };
        const auto padString = [&stringVal](const auto thing, const int width) {
            const std::string val = stringVal(thing);
            if (width < 0) { return val + std::string(std::max(0, -width - static_cast<int>(val.size())), ' '); }
            return std::string(std::max(0, width - static_cast<int>(val.size())), ' ') + val;
        };
        const auto printOuterLine = [=] {
            std::size_t total = 4;
            for (std::size_t ind = 0; ind < _lengths.size(); ++ind) {
                total += std::abs(_lengths[ind]) + 4;
                if (ind + 1 < _lengths.size()) ++total;
            }

            std::cout << std::string(total, '=') << "\n";
        };
        const auto printInnerLine = [=] {
            std::cout << "||";
            for (std::size_t ind = 0; ind < _lengths.size(); ++ind) {
                std::cout << std::string(std::abs(_lengths[ind]) + 4, '-');
                if (ind + 1 < _lengths.size()) std::cout << "+";
            }
            std::cout << "||\n";
        };
        const auto printLine = [&](const auto... args) {
            std::cout << "||";
            // ReSharper disable once CppDFAUnreadVariable
            // ReSharper disable once CppDFAUnusedValue
            std::size_t ind = 0;

            (
                    [&](const auto arg) {
                        std::cout << "  " << padString(arg, _lengths[ind++]) << "  ";
                        if (ind < _lengths.size()) std::cout << "|";
                    }(args),
                    ...);

            std::cout << "||\n";
        };
        const auto printTime = [&](const auto name) {
            _curTime = std::chrono::steady_clock::now();
            printLine(name, std::chrono::duration_cast<std::chrono::milliseconds>(_curTime - _lastTime).count(),
                      std::chrono::duration_cast<std::chrono::milliseconds>(_curTime - _startTime).count());
            _lastTime = _curTime;
        };

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

        const std::unordered_map lr0ItemClosure {[&] {
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

            const auto handleRule = [&](const GrammarRule& rule) {
                for (std::size_t ind = 0; ind <= rule.components.size(); ++ind) calcClosure({&rule, ind});
            };

            handleRule(goalRule);
            for (const auto& rule : grammar.rules) handleRule(rule);

            return closure;
        }()};

        const auto lr0SetClosure = [&](const Lr0Kernel& kernel) {
            Lr0Closure closure {kernel};
            for (const auto& item : kernel) {
                const auto& itemClosure = lr0ItemClosure.at(item);
                closure.insert(itemClosure.begin(), itemClosure.end());
            }
            return closure;
        };

        const std::unordered_map lr0StateNums {[&] {
            std::unordered_map<Lr0Kernel, std::size_t> states;

            const Lr0Kernel initialState {{&goalRule, 0}};
            std::queue<Lr0Kernel> pending;
            pending.push(initialState);
            states[initialState] = 0;

            while (!pending.empty()) {
                const Lr0Kernel curState = std::move(pending.front());
                pending.pop();

                std::unordered_map<GrammarEntry, Lr0Kernel> nextKernels;

                for (const auto& [rule, index] : lr0SetClosure(curState)) {
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
        }()};
        const std::vector lr0States {[&] {
            std::vector<Lr0Kernel> states(lr0StateNums.size());

            for (const auto& [kernel, stateNum] : lr0StateNums) states[stateNum] = kernel;

            return states;
        }()};

        printTime("1: LR(0) sets");
        printInnerLine();


        // =======================================
        // ||  2: Determine initial lookaheads  ||
        // =======================================

        Util::GeneratedMap<Lr1Item, Lr1Closure> lr1ItemClosure {[&](const Lr1Item& lr1Item) {
            Lr1Closure closure {lr1Item};
            std::vector lr1Items {lr1Item};

            for (std::size_t index = 0; index < lr1Items.size(); ++index) {
                const Lr1Item& currentLr1Item = lr1Items[index];
                if (currentLr1Item.index == currentLr1Item.rule->components.size()) continue;
                const std::vector<GrammarEntry> remaining {currentLr1Item.rule->components.begin()
                                                                   + static_cast<long>(currentLr1Item.index),
                                                           currentLr1Item.rule->components.end()};
                const GrammarEntry& currentItem = currentLr1Item.rule->components[currentLr1Item.index];
                if (currentItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;

                const auto lookaheads = [&]() -> std::unordered_set<TokenType> {
                    if (remaining.size() < 2) return {currentLr1Item.lookahead};

                    if (remaining[1].grammarEntryType == GrammarEntry::TOKEN_TYPE) return {remaining[1].tokenType};
                    return firstSet.at(remaining[1].nodeType);
                }();

                if (nodeGenerators.count(currentItem.nodeType) > 0) {
                    for (const auto& production : nodeGenerators.at(currentItem.nodeType)) {
                        for (const auto& lookahead : lookaheads) {
                            const Lr1Item newLr1Item {production, 0, lookahead};
                            if (closure.count(newLr1Item) > 0) continue;
                            closure.insert(newLr1Item);
                            lr1Items.push_back(newLr1Item);
                        }
                    }
                }
            }

            return closure;
        }};

        std::unordered_map lalrStateNums {[&] {
            std::unordered_map<Lr0Kernel, std::unordered_map<Lr0Item, std::unordered_set<TokenType>>> lookaheads;
            for (std::size_t ind = 0; ind <= goalRule.components.size(); ++ind) {
                const Lr0Item goalItem {&goalRule, ind};
                lookaheads[{goalItem}][goalItem] = {TokenType::END_OF_FILE};
            }

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

                bool operator==(const Propagation& other) const {
                    return this->fromItem == other.fromItem && this->fromState == other.fromState
                        && this->toState == other.toState;
                }
            };
            std::unordered_set<Propagation, Propagation::Hash> propagations;

            for (const auto& lr0Kernel : lr0States) {
                const auto lr0State = lr0SetClosure(lr0Kernel);

                for (const auto& item : lr0Kernel) {
                    if (item.index == item.rule->components.size()) continue;

                    constexpr auto specialLookahead = TokenType::END;
                    const auto& specialClosure = lr1ItemClosure[{item, specialLookahead}];

                    for (const auto& enclosed : specialClosure) {
                        if (enclosed.index == enclosed.rule->components.size()) continue;

                        const Lr0Item nextItem {enclosed.rule, enclosed.index + 1};

                        const auto nextKernel = lr0Goto(lr0State, enclosed.rule->components[enclosed.index]);
                        const auto nextStateNum = lr0StateNums.at(nextKernel);
                        for (const auto& nextItem : nextKernel) {
                            if (enclosed.lookahead == specialLookahead) {
                                // Propagate lookaheads from `enclosed` to `nextItem` in `nextKernel`.
                                propagations.insert({lr0Kernel, item, nextKernel, nextItem});
                            } else {
                                // Add `enclosed.lookahead` to `nextItem` in `nextKernel`.
                                lookaheads[nextKernel][nextItem].insert(enclosed.lookahead);
                            }
                        }
                    }
                }
            }

            printTime("2: Initial lookaheads");
            printInnerLine();


            // ===============================
            // ||  3: Propagate lookaheads  ||
            // ===============================

            int iteration = 0;
            bool changed = true;
            while (changed) {
                std::cout << "--------------------------------------------------------------------------------\n"
                          << "Iteration " << iteration++ << ":\n";
                for (std::size_t stateNum = 0; stateNum < lr0States.size(); ++stateNum) {
                    std::cout << "    - State " << stateNum << "/" << lr0States.size() - 1 << ":";
                    std::cout << "\n";

                    const auto& kernel = lr0States[stateNum];
                    for (const auto& item : kernel) {
                        //
                        std::stringstream itemLookaheads;
                        itemLookaheads << "{";
                        for (const auto& lookahead : lookaheads[kernel][item])
                            itemLookaheads << " " << GrammarEntry {lookahead};
                        itemLookaheads << " }";
                        std::cout << "        - " << padString(item, -50) << ": " << padString(itemLookaheads.str(), 80) << "\n";
                    }
                }

                changed = false;
                for (const auto& [fromState, fromItem, toState, toItem] : propagations) {
                    const auto fromLookaheads = lookaheads[fromState][fromItem];
                    auto& toLookaheads = lookaheads[toState][toItem];
                    for (const auto fromLookahead : fromLookaheads) {
                        if (toLookaheads.insert(fromLookahead).second) changed = true;
                    }
                }
            }

            exit(0);

            std::unordered_map<LalrKernel, std::size_t> states;
            //  . . .
            for (const auto& [lr0Kernel, stateNum] : lr0StateNums) {
                LalrKernel newKernel;
                std::cout << "State " << stateNum << ":\n"
                          << "    - LR(0) items:\n";
                for (const auto& item : lr0States[stateNum]) std::cout << "        - " << item << "\n";
                std::cout << "    - LALR(1) items:\n";
                for (const auto& [lr0Item, itemLookaheads] : lookaheads[lr0Kernel]) {
                    const LalrItem newItem {lr0Item, {itemLookaheads.begin(), itemLookaheads.end()}};
                    std::cout << "        - " << newItem << "\n";
                    newKernel.insert(newItem);
                }
                states[newKernel] = stateNum;
            }
            return states;
        }()};
        std::vector lalrStates {[&] {
            std::vector<LalrKernel> states(lalrStateNums.size());
            for (const auto& [kernel, stateNum] : lalrStateNums) states[stateNum] = kernel;
            return states;
        }()};

        printTime("3: Apply lookaheads");
        printInnerLine();


        // ================================
        // ||  4: Compute table actions  ||
        // ================================

        // TODO: This.
        for (std::size_t ind = 0; ind < lalrStates.size(); ++ind) {
            std::cout << "State " << ind + 1 << "/" << lalrStates.size() << ":\n";
            for (const auto& item : lalrStates[ind]) std::cout << "    - " << item << "\n";
        }

        printTime("4: Compute actions");

        printOuterLine();

        exit(0);

        /*

        constexpr std::size_t colWidth = 10;
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
        std::cout << "=====================================================\n"
                  << "||     Stage     |  Elapsed time  |   Total time   ||\n"
                  << "=====================================================\n"
                  << "||  0: begin     "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(), colWidth)
                  << "ms  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count(),
                            colWidth)
                  << "ms  ||\n"
                  << "||---------------+----------------+----------------||\n";
        lastTime = curTime;

        // Reports the set of all token types that can be the first of the given AST node type.
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

        curTime = std::chrono::steady_clock::now();
        std::cout << "||  1: firstSet  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(), colWidth)
                  << "ms  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count(),
                            colWidth)
                  << "ms  ||\n"
                  << "||---------------+----------------+----------------||\n";
        lastTime = curTime;

        const std::unordered_map lr1ItemClosure {[&] {
            Util::SafePtr<std::unordered_map<Lr1Item, Lr1ItemSet>> closure;

            Util::SafePtr<std::unordered_set<Lr1Item>> processed;
            Util::ThreadQueue jobs;

            const std::function<void(const GrammarRule*, TokenType)> handle = [&](const GrammarRule* const rule,
                                                                                  const TokenType lookahead) {
                for (std::size_t ind = 0; ind <= rule->components.size(); ++ind) {
                    const Lr1Item item {rule, ind, lookahead};
                    Lr1ItemSet itemClosure {item};
                    std::queue<Lr1Item> toExamine;
                    toExamine.push(item);

                    while (!toExamine.empty()) {
                        const auto curItem = toExamine.front();
                        toExamine.pop();

                        if (curItem.index == curItem.rule->components.size()) continue;

                        const auto nextEntry = curItem.rule->components[curItem.index];
                        if (nextEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;

                        const auto lookaheads = [&, curItem]() -> std::unordered_set<TokenType> {
                            if (curItem.index + 1 == curItem.rule->components.size()) return {curItem.lookahead};

                            const auto followEntry = curItem.rule->components[curItem.index + 1];
                            if (followEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE)
                                return {followEntry.tokenType};

                            return firstSet.at(followEntry.nodeType);
                        }();

                        if (nodeGenerators.count(nextEntry.nodeType) == 0) continue;
                        for (const auto* generator : nodeGenerators.at(nextEntry.nodeType)) {
                            for (const auto nextToken : lookaheads) {
                                const Lr1Item newItem {generator, 0, nextToken};

                                if (itemClosure.count(newItem) > 0) continue;

                                itemClosure.insert(newItem);
                                toExamine.push(newItem);

                                if (processed->count(newItem) > 0) continue;
                                processed->insert(newItem);

                                jobs.add(handle, generator, nextToken);
                            }
                        }
                    }
                    (*closure)[item] = itemClosure;
                }
            };

            processed->emplace(&goalRule, 0, TokenType::END_OF_FILE);
            jobs.add(handle, &goalRule, TokenType::END_OF_FILE);

            jobs.wait();

            return static_cast<std::unordered_map<Lr1Item, Lr1ItemSet>>(*closure);
        }()};

        curTime = std::chrono::steady_clock::now();
        std::cout << "||  2: closure   "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(), colWidth)
                  << "ms  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count(),
                            colWidth)
                  << "ms  ||\n"
                  << "||---------------+----------------+----------------||\n";
        lastTime = curTime;

        // const std::unordered_map lr1ItemClosure {[&] {
        //     SafePtr<std::unordered_set<LR1Item>> processed;
        //     std::queue<LR1Item> toProcess;
        //     const auto process = [&](const GrammarRule* rule, const TokenType lookahead) {
        //         const LR1Item newItem {rule, 0, lookahead};
        //         if (processed->count(newItem) > 0) return;
        //         if (!processed->insert(newItem).second) {
        //             std::cerr << "[[" << newItem << "]]\n";
        //             exit(1);
        //         }
        //
        //         for (std::size_t ind = 0; ind <= rule->components.size(); ++ind) {
        //             toProcess.emplace(rule, ind, lookahead);
        //         }
        //     };
        //
        //     process(&goalRule, TokenType::END_OF_FILE);
        //
        //     std::unordered_map<LR1Item, LR1ItemSet> closure;
        //
        //     while (!toProcess.empty()) {
        //         const auto item = toProcess.front();
        //         toProcess.pop();
        //
        //         std::cout << "\n--------------------------------------------------------------------------------\n";
        //         std::cout << item << "  ||  " << toProcess.size() << "\n";
        //
        //         std::queue<LR1Item> toExamine;
        //         toExamine.push(item);
        //         closure[item] = {item};
        //
        //         while (!toExamine.empty()) {
        //             const auto curItem = toExamine.front();
        //             toExamine.pop();
        //
        //             std::cout << "    - " << curItem << "\n";
        //
        //             if (curItem.index == curItem.rule->components.size()) continue;
        //
        //             const auto nextEntry = curItem.rule->components[curItem.index];
        //             if (nextEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;
        //
        //             const auto lookaheads = [&, curItem]() -> std::unordered_set<TokenType> {
        //                 if (curItem.index + 1 == curItem.rule->components.size()) return {curItem.lookahead};
        //
        //                 const auto followEntry = curItem.rule->components[curItem.index + 1];
        //                 if (followEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) return {followEntry.tokenType};
        //
        //                 return firstSet.at(followEntry.nodeType);
        //             }();
        //
        //             if (nodeGenerators.count(nextEntry.nodeType) == 0) continue;
        //             for (const auto* generator : nodeGenerators.at(nextEntry.nodeType)) {
        //                 for (const auto lookahead : lookaheads) {
        //                     const LR1Item newItem {generator, 0, lookahead};
        //
        //                     if (closure[item].count(newItem) > 0) continue;
        //
        //                     std::cout << "        - " << newItem << "\n";
        //
        //                     if (!closure[item].insert(newItem).second) {
        //                         std::cerr << "[[" << newItem << "]]\n";
        //                         exit(1);
        //                     }
        //                     toExamine.push(newItem);
        //                     process(generator, lookahead);
        //                 }
        //             }
        //         }
        //     }
        //
        //     return closure;
        // }()};

        // const std::unordered_map lr1ItemClosure {[&] {
        //     std::unordered_set<LR1Item> processed;
        //     std::queue<LR1Item> toProcess;
        //     const auto process = [&](const GrammarRule* rule, const TokenType lookahead) {
        //         const LR1Item newItem {rule, 0, lookahead};
        //         if (processed.count(newItem) > 0) return;
        //         if (!processed.insert(newItem).second) {
        //             std::cerr << "[[" << newItem << "]]\n";
        //             exit(1);
        //         }
        //
        //         for (std::size_t ind = 0; ind <= rule->components.size(); ++ind) {
        //             toProcess.emplace(rule, ind, lookahead);
        //         }
        //     };
        //
        //     process(&goalRule, TokenType::END_OF_FILE);
        //
        //     std::unordered_map<LR1Item, LR1ItemSet> closure;
        //
        //     while (!toProcess.empty()) {
        //         const auto item = toProcess.front();
        //         toProcess.pop();
        //
        //         // std::cout <<
        //         "\n--------------------------------------------------------------------------------\n";
        //         // std::cout << item << "  ||  " << toProcess.size() << "\n";
        //         std::cout << toProcess.size() << "\n";
        //
        //         std::queue<LR1Item> toExamine;
        //         toExamine.push(item);
        //         closure[item] = {item};
        //
        //         while (!toExamine.empty()) {
        //             const auto curItem = toExamine.front();
        //             toExamine.pop();
        //
        //             // std::cout << "    - " << curItem << "\n";
        //
        //             if (curItem.index == curItem.rule->components.size()) continue;
        //
        //             const auto nextEntry = curItem.rule->components[curItem.index];
        //             if (nextEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;
        //
        //             const auto lookaheads = [&, curItem]() -> std::unordered_set<TokenType> {
        //                 if (curItem.index + 1 == curItem.rule->components.size()) return {curItem.lookahead};
        //
        //                 const auto followEntry = curItem.rule->components[curItem.index + 1];
        //                 if (followEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) return {followEntry.tokenType};
        //
        //                 return firstSet.at(followEntry.nodeType);
        //             }();
        //
        //             if (nodeGenerators.count(nextEntry.nodeType) == 0) continue;
        //             for (const auto* generator : nodeGenerators.at(nextEntry.nodeType)) {
        //                 for (const auto lookahead : lookaheads) {
        //                     const LR1Item newItem {generator, 0, lookahead};
        //
        //                     if (closure[item].count(newItem) > 0) continue;
        //
        //                     // std::cout << "        - " << newItem << "\n";
        //
        //                     if (!closure[item].insert(newItem).second) {
        //                         std::cerr << "[[" << newItem << "]]\n";
        //                         exit(1);
        //                     }
        //                     toExamine.push(newItem);
        //                     process(generator, lookahead);
        //                 }
        //             }
        //         }
        //     }
        //
        //     return closure;
        // }()};

        // Util::GeneratedMap<LR1Item, LR1ItemSet> lr1ItemClosure {[&](const LR1Item& lr1Item) {
        //     LR1ItemSet usedLr1Items {lr1Item};
        //     std::vector lr1ItemVec {lr1Item};
        //
        //     for (std::size_t index = 0; index < lr1ItemVec.size(); ++index) {
        //         const LR1Item& currentLR1Item = lr1ItemVec[index];
        //         if (currentLR1Item.index == currentLR1Item.rule->components.size()) continue;
        //         const std::vector<GrammarEntry> remaining {currentLR1Item.rule->components.begin()
        //                                                            + static_cast<long>(currentLR1Item.index),
        //                                                    currentLR1Item.rule->components.end()};
        //         const GrammarEntry& currentItem = currentLR1Item.rule->components[currentLR1Item.index];
        //         if (currentItem.grammarEntryType == GrammarEntry::TOKEN_TYPE) continue;
        //
        //         const auto lookaheads = [&]() -> std::unordered_set<TokenType> {
        //             if (remaining.size() < 2) return {currentLR1Item.lookahead};
        //
        //             if (remaining[1].grammarEntryType == GrammarEntry::TOKEN_TYPE) return {remaining[1].tokenType};
        //             return firstSet.at(remaining[1].nodeType);
        //         }();
        //
        //         if (nodeGenerators.count(currentItem.nodeType) > 0) {
        //             for (const auto& production : nodeGenerators.at(currentItem.nodeType)) {
        //                 for (const auto& lookahead : lookaheads) {
        //                     const LR1Item newLR1Item {production, 0, lookahead};
        //                     if (usedLr1Items.count(newLR1Item) > 0) continue;
        //                     usedLr1Items.insert(newLR1Item);
        //                     lr1ItemVec.push_back(newLR1Item);
        //                 }
        //             }
        //         }
        //     }
        //
        //     return usedLr1Items;
        // }};

        Util::GeneratedMap<Lr1ItemSet, Lr1ItemSet> lr1SetClosure {[&](const Lr1ItemSet& items) {
            Lr1ItemSet result;
            for (const auto& item : items) {
                // if (lr1ItemClosure.count(item) == 0) std::cerr << "<<<<" << item << ">>>>\n";
                // result.insert(lr1ItemClosure.at(item).begin(), lr1ItemClosure.at(item).end());
            }
            return result;
        }};

        std::unordered_set<std::size_t> lr1StateHashes;
        Util::GeneratedMap<Lr0ItemSet, std::size_t> lr0StateNums {
                [&](const Lr0ItemSet&) { return lr0StateNums.size(); }};

        Util::GeneratedMap<Lr1ItemSet, std::size_t> lr1StateNums {[&](const Lr1ItemSet& lr1Items) {
            Lr0ItemSet lr0Items;
            for (const auto& lr1Item : lr1Items) lr0Items.insert(lr1Item);
            return lr0StateNums[lr0Items];
        }};

        std::queue<Lr1ItemSet> statesToProcess;

        const auto addState = [&](const Lr1ItemSet& lr1Items) {
            const auto nextState = lr1SetClosure[lr1Items];
            const std::size_t hash = std::hash<Lr1ItemSet> {}(nextState);
            if (lr1StateHashes.count(hash) == 0) {
                lr1StateHashes.insert(hash);
                statesToProcess.push(nextState);
            }
            return lr1StateNums[nextState];
        };

        // const Lr1ItemSet initialState = lr1ItemClosure.at({&goalRule, 0, TokenType::END_OF_FILE});
        // lr1StateHashes.insert(std::hash<Lr1ItemSet> {}(initialState));
        // statesToProcess.push(initialState);

        curTime = std::chrono::steady_clock::now();
        std::cout << "||  3: misc.     "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(), colWidth)
                  << "ms  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count(),
                            colWidth)
                  << "ms  ||\n"
                  << "||---------------+----------------+----------------||\n";
        lastTime = curTime;

        // std::cout <<
        // "\n\n\n\n================================================================================\n\n\n\n";

        while (!statesToProcess.empty()) {
            const auto state = std::move(statesToProcess.front());
            statesToProcess.pop();

            const auto stateNum = lr1StateNums[state];

            // std::cout << stateNum << "  ||  " << lr0StateNums.size() << "  ||  " << statesToProcess.size() << "\n";
            // std::cout << "--------------------------------------------------------------------------------\n"
            // << "State: " << stateNum << "\n"
            // << "    " << state.size() << " LR(1) items:\n";
            // for (const auto& item : state) std::cout << "        - " << item << "\n";

            struct ShiftAction {
                Lr1ItemSet lr1Items;
                std::size_t priority {0};
            };
            std::unordered_map<TokenType, ShiftAction> shifts;

            std::unordered_map<TokenType, const GrammarRule*> reductions;

            std::unordered_map<ASTNodeType, Lr1ItemSet> gotoTable;

            for (const auto& lr1Item : state) {
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
            for (const auto& [nodeType, lr1Items] : gotoTable) _nextState[stateNum][nodeType] = addState(lr1Items);

            // Next, the "shift" actions:
            for (const auto& [tokenType, shiftAction] : shifts) {
                _nextAction[stateNum][tokenType] = _pair("ParserAction::ActionType::SHIFT",
                                                         std::to_string(addState(shiftAction.lr1Items)));
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
        _numStates = lr0StateNums.size();

        curTime = std::chrono::steady_clock::now();
        std::cout << "||  4: stateGen  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count(), colWidth)
                  << "ms  "
                  << "|  "
                  << getStr(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count(),
                            colWidth)
                  << "ms  ||\n"
                  << "=====================================================\n";

        std::cout << "[[Total number of states: " << _numStates << "]]\n";
        */
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