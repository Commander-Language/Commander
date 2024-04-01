/**
 * @file source/parser/generator/grammar.hpp
 * @brief Contains the definition of the Commander grammar.
 */

#include "grammar.hpp"

#include "source/parser/ast_node.hpp"
#include "source/util/combine_hashes.hpp"

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// ReSharper disable CppRedundantQualifier

namespace Parser {
    GrammarEntry::GrammarEntry(const Grammar::TokenType tokenType)
        : grammarEntryType(TOKEN_TYPE), tokenType(tokenType), nodeType() {}

    GrammarEntry::GrammarEntry(const ASTNodeType nodeType)
        : grammarEntryType(NODE_TYPE), tokenType(), nodeType(nodeType) {}

    bool GrammarEntry::operator==(const GrammarEntry& other) const {
        if (this->grammarEntryType != other.grammarEntryType) return false;
        if (this->grammarEntryType == TOKEN_TYPE) return this->tokenType == other.tokenType;
        return this->nodeType == other.nodeType;
    }

    bool GrammarEntry::operator!=(const GrammarEntry& other) const { return !(*this == other); }

    bool GrammarEntry::operator<(const GrammarEntry& other) const {
        if (this->grammarEntryType != other.grammarEntryType) return this->grammarEntryType < other.grammarEntryType;
        if (this->grammarEntryType == TOKEN_TYPE) return this->tokenType < other.tokenType;
        return this->nodeType < other.nodeType;
    }

    std::ostream& operator<<(std::ostream& stream, const GrammarEntry& grammarEntry) {
        if (grammarEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) {
            stream << "[" << tokenTypeToString(grammarEntry.tokenType) << "]";
        } else {
            stream << "(" << nodeTypeToString(grammarEntry.nodeType) << ")";
        }
        return stream;
    }

    GrammarRule::GrammarRule() : result(), priority() {}

    GrammarRule::GrammarRule(const ASTNodeType result, const std::vector<GrammarEntry>& components,
                             const std::size_t priority)
        : result(result), components(components), priority(priority) {}

    bool GrammarRule::operator==(const GrammarRule& other) const {
        if (this->result != other.result) return false;

        if (this->components.size() != other.components.size()) return false;

        for (std::size_t ind = 0; ind < this->components.size(); ++ind) {
            if (this->components[ind] != other.components[ind]) return false;
        }

        return true;
    }

    bool GrammarRule::operator!=(const GrammarRule& other) const { return !(*this == other); }

    bool GrammarRule::operator<(const GrammarRule& other) const {
        if (this->priority != other.priority) return this->priority < other.priority;
        if (this->result != other.result) return this->result < other.result;
        if (this->components.size() != other.components.size()) return this->components.size() < components.size();
        for (std::size_t ind = 0; ind < this->components.size(); ++ind) {
            if (this->components[ind] != other.components[ind]) return this->components[ind] < other.components[ind];
        }
        return false;
    }

    std::ostream& operator<<(std::ostream& stream, const GrammarRule& grammarRule) {
        stream << "{(" << nodeTypeToString(grammarRule.result) << ") -> ";

        if (!grammarRule.components.empty()) {
            for (const auto& component :
                 std::vector<GrammarEntry> {grammarRule.components.begin(), grammarRule.components.end() - 1}) {
                stream << component << " ";
            }
            stream << grammarRule.components.back();
        }
        stream << "}";

        return stream;
    }

    Grammar::Grammar() : Grammar(_defineGrammar()) {}

    Grammar::Grammar(const std::vector<std::tuple<GrammarRule, NodeConstructor>>& grammarDefinitions)
        : rules([&] {
              std::vector<GrammarRule> result(grammarDefinitions.size());
              std::size_t index = 0;
              for (const auto& item : grammarDefinitions) result[index++] = std::get<0>(item);
              return result;
          }()),
          reductions([&] {
              std::unordered_map<GrammarRule, NodeConstructor> result;
              for (const auto& item : grammarDefinitions) result[std::get<0>(item)] = std::get<1>(item);
              return result;
          }()) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "*-avoid-magic-numbers"
    std::vector<std::tuple<GrammarRule, Grammar::NodeConstructor>> Grammar::_defineGrammar() {
        const auto fold = [](const std::vector<std::string>& strings) -> std::string {
            if (strings.empty()) return "";
            const std::string delim = ", ";
            std::string result;
            for (std::size_t index = 0; index < strings.size() - 1; ++index) result += strings[index] + delim;
            result += strings[strings.size() - 1];
            return result;
        };
        const auto callFunc = [&](const std::string& func, const std::vector<std::string>& args) -> std::string {
            return func + "(" + fold(args) + ")";
        };
        const auto makeNode = [&](const std::string& nodeType, const std::vector<std::string>& args) -> std::string {
            return callFunc("std::make_shared<" + nodeType + "Node>", args);
        };
        const auto castNode = [&](const std::string& nodeType, const std::size_t index) -> std::string {
            return callFunc("cast" + nodeType, {"productionList[" + std::to_string(index) + "].node"});
        };
        const auto tokenContents = [](const std::size_t index) -> std::string {
            return "productionList[" + std::to_string(index) + "].token->contents";
        };
        const auto tokenPosition = [](const std::size_t index) -> std::string {
            return "productionList[" + std::to_string(index) + "].token->position";
        };
        const std::string firstTokenPosition = tokenPosition(0);
        const std::string firstNodePosition = "productionList[0].node->position";
        const auto flatten =
                [&](const std::vector<std::vector<std::tuple<GrammarRule, Grammar::NodeConstructor>>>& rules)
                -> std::vector<std::tuple<GrammarRule, Grammar::NodeConstructor>> {
            std::vector<std::tuple<GrammarRule, Grammar::NodeConstructor>> result;
            std::size_t precedenceVal = 0;
            for (const auto& precedenceSet : rules) {
                ++precedenceVal;
                for (const auto& [rule, nodeConstructor] : precedenceSet) {
                    result.emplace_back(GrammarRule {rule.result, rule.components, precedenceVal}, nodeConstructor);
                }
            }
            return result;
        };

        return flatten({
                // ================
                // ||  Program:  ||
                // ================

                // (PRGM) -> (STMTS)
                {{{ASTNodeType::PRGM, {ASTNodeType::STMTS}}, makeNode("Prgm", {castNode("Stmts", 0)})}},

                // (STMT) -> [LCURLY] [RCURLY]
                {{{ASTNodeType::STMT, {TokenType::LCURLY, TokenType::RCURLY}},
                  makeNode("ScopeStmt", {firstTokenPosition})}},
                // (STMT) -> [LCURLY] (STMTS) [RCURLY]
                {{{ASTNodeType::STMT, {TokenType::LCURLY, ASTNodeType::STMTS, TokenType::RCURLY}},
                  makeNode("ScopeStmt", {firstTokenPosition, castNode("Stmts", 1)})}},


                // =================
                // ||  Bindings:  ||
                // =================

                // (BINDINGS) -> (BINDING)
                {{{ASTNodeType::BINDINGS, {ASTNodeType::BINDING}}, makeNode("Bindings", {castNode("Binding", 0)})}},
                // (BINDINGS) -> (BINDINGS) [COMMA] (BINDING)
                {{{ASTNodeType::BINDINGS, {ASTNodeType::BINDINGS, TokenType::COMMA, ASTNodeType::BINDING}},
                  makeNode("Bindings", {castNode("Bindings", 0), castNode("Binding", 2)})}},

                // (BINDING) -> [CONST] [VARIABLE] [COLON] (TYPE)
                {{{ASTNodeType::BINDING, {TokenType::CONST, TokenType::VARIABLE, TokenType::COLON, ASTNodeType::TYPE}},
                  makeNode("Binding", {firstTokenPosition, tokenContents(1), "true", castNode("Type", 3)})}},
                // (BINDING) -> [VARIABLE] [COLON] (TYPE)
                {{{ASTNodeType::BINDING, {TokenType::VARIABLE, TokenType::COLON, ASTNodeType::TYPE}},
                  makeNode("Binding", {firstTokenPosition, tokenContents(0), "false", castNode("Type", 2)})}},
                // (BINDING) -> [CONST] [VARIABLE]
                {{{ASTNodeType::BINDING, {TokenType::CONST, TokenType::VARIABLE}},
                  makeNode("Binding", {firstTokenPosition, tokenContents(1), "true"})}},
                // (BINDING) -> [VARIABLE]
                {{{ASTNodeType::BINDING, {TokenType::VARIABLE}},
                  makeNode("Binding", {firstTokenPosition, tokenContents(0)})}},


                // =================
                // ||  Commands:  ||
                // =================

                // (BASIC_CMD) -> [CMDSTRINGVAL]
                {{{ASTNodeType::BASIC_CMD, {TokenType::CMDSTRINGVAL}},
                  makeNode("BasicCmd", {makeNode("String", {firstTokenPosition, tokenContents(0)})})}},
                // (BASIC_CMD) -> [CMDVARIABLE]
                {{{ASTNodeType::BASIC_CMD, {TokenType::CMDVARIABLE}},
                  makeNode("BasicCmd", {makeNode("VarExpr", {firstTokenPosition, tokenContents(0)})})}},
                // (BASIC_CMD) -> (STRING)
                {{{ASTNodeType::BASIC_CMD, {ASTNodeType::STRING}}, makeNode("BasicCmd", {castNode("String", 0)})}},
                // LA: (BASIC_CMD) -> (BASIC_CMD) (BASIC_CMD)
                {{{ASTNodeType::BASIC_CMD, {ASTNodeType::BASIC_CMD, ASTNodeType::BASIC_CMD}},
                  makeNode("BasicCmd", {castNode("BasicCmd", 0), castNode("BasicCmd", 1)})}},

                // LA: (PIPE_CMD) -> (PIPE_CMD) [PIPE] (BASIC_CMD)
                {{{ASTNodeType::PIPE_CMD, {ASTNodeType::PIPE_CMD, TokenType::PIPE, ASTNodeType::BASIC_CMD}},
                  makeNode("PipeCmd", {castNode("PipeCmd", 0), castNode("BasicCmd", 2)})}},
                // LA: (PIPE_CMD) -> (BASIC_CMD) [PIPE] (BASIC_CMD)
                {{{ASTNodeType::PIPE_CMD, {ASTNodeType::BASIC_CMD, TokenType::PIPE, ASTNodeType::BASIC_CMD}},
                  makeNode("PipeCmd", {castNode("BasicCmd", 0), castNode("BasicCmd", 2)})}},

                // (ASYNC_CMD) -> (PIPE_CMD) [AMPERSAND]
                {{{ASTNodeType::ASYNC_CMD, {ASTNodeType::PIPE_CMD, TokenType::AMPERSAND}},
                  makeNode("AsyncCmd", {castNode("PipeCmd", 0)})}},
                // (ASYNC_CMD) -> (BASIC_CMD) [AMPERSAND]
                {{{ASTNodeType::ASYNC_CMD, {ASTNodeType::BASIC_CMD, TokenType::AMPERSAND}},
                  makeNode("AsyncCmd", {castNode("BasicCmd", 0)})}},


                // ================
                // ||  Lvalues:  ||
                // ================

                // (LVALUE) -> [VARIABLE]
                {{{ASTNodeType::LVALUE, {TokenType::VARIABLE}},
                  makeNode("VarLValue", {firstTokenPosition, tokenContents(0)})}},
                // (LVALUE) -> (LVALUE) [LSQUARE] (EXPR) [RSQUARE]
                {{{ASTNodeType::LVALUE,
                   {ASTNodeType::LVALUE, TokenType::LSQUARE, ASTNodeType::EXPR, TokenType::RSQUARE}},
                  makeNode("IndexLValue", {castNode("LValue", 0), castNode("Expr", 2)})}},


                // ====================
                // ||  Expressions:  ||
                // ====================

                // (EXPR) -> [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  "productionList[1].node"}},

                // (EXPRS) -> (EXPR)
                {{{ASTNodeType::EXPRS, {ASTNodeType::EXPR}}, makeNode("Exprs", {castNode("Expr", 0)})}},
                // (EXPRS) -> (EXPRS) [COMMA] (EXPR)
                {{{ASTNodeType::EXPRS, {ASTNodeType::EXPRS, TokenType::COMMA, ASTNodeType::EXPR}},
                  makeNode("Exprs", {castNode("Exprs", 0), castNode("Expr", 2)})}},

                // (EXPR) -> (EXPR) [DOT] [VARIABLE] [LPAREN] [RPAREN]
                {{{ASTNodeType::EXPR,
                   {ASTNodeType::EXPR, TokenType::DOT, TokenType::VARIABLE, TokenType::LPAREN, TokenType::RPAREN}},
                  makeNode("ApiCallExpr", {castNode("Expr", 0), tokenContents(2)})}},
                // (EXPR) -> (EXPR) [DOT] [VARIABLE] [LPAREN] (EXPRS) [RPAREN]
                {{{ASTNodeType::EXPR,
                   {ASTNodeType::EXPR, TokenType::DOT, TokenType::VARIABLE, TokenType::LPAREN, ASTNodeType::EXPRS,
                    TokenType::RPAREN}},
                  makeNode("ApiCallExpr", {castNode("Expr", 0), tokenContents(2), castNode("Exprs", 4)})}},

                // (EXPR) -> (EXPR) [LPAREN] [RPAREN]
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LPAREN, TokenType::RPAREN}},
                  makeNode("CallExpr", {castNode("Expr", 0)})}},
                // (EXPR) -> (EXPR) [LPAREN] (EXPRS) [RPAREN]
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LPAREN, ASTNodeType::EXPRS, TokenType::RPAREN}},
                  makeNode("CallExpr", {castNode("Expr", 0), castNode("Exprs", 2)})}},

                // (EXPR) -> [INTVAL]
                {{{ASTNodeType::EXPR, {TokenType::INTVAL}},
                  makeNode("IntExpr", {firstTokenPosition, callFunc("std::stoll", {tokenContents(0)})})}},
                // (EXPR) -> [FLOATVAL]
                {{{ASTNodeType::EXPR, {TokenType::FLOATVAL}},
                  makeNode("FloatExpr", {firstTokenPosition, callFunc("std::stold", {tokenContents(0)})})}},
                // (EXPR) -> [TRUE]
                {{{ASTNodeType::EXPR, {TokenType::TRUE}}, makeNode("BoolExpr", {firstTokenPosition, "true"})}},
                // (EXPR) -> [FALSE]
                {{{ASTNodeType::EXPR, {TokenType::FALSE}}, makeNode("BoolExpr", {firstTokenPosition, "false"})}},

                // (EXPR) -> (STRING)
                {{{ASTNodeType::EXPR, {ASTNodeType::STRING}}, makeNode("StringExpr", {castNode("String", 0)})}},

                // (EXPR) -> [BACKTICK] (ASYNC_CMD) [BACKTICK]
                {{{ASTNodeType::EXPR, {TokenType::BACKTICK, ASTNodeType::ASYNC_CMD, TokenType::BACKTICK}},
                  makeNode("CmdExpr", {firstTokenPosition, castNode("AsyncCmd", 1)})}},
                // (EXPR) -> [BACKTICK] (PIPE_CMD) [BACKTICK]
                {{{ASTNodeType::EXPR, {TokenType::BACKTICK, ASTNodeType::PIPE_CMD, TokenType::BACKTICK}},
                  makeNode("CmdExpr", {firstTokenPosition, castNode("PipeCmd", 1)})}},
                // (EXPR) -> [BACKTICK] (BASIC_CMD) [BACKTICK]
                {{{ASTNodeType::EXPR, {TokenType::BACKTICK, ASTNodeType::BASIC_CMD, TokenType::BACKTICK}},
                  makeNode("CmdExpr", {firstTokenPosition, castNode("BasicCmd", 1)})}},

                // (EXPR) -> (EXPR) [LSQUARE] (EXPR) [RSQUARE]
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LSQUARE, ASTNodeType::EXPR, TokenType::RSQUARE}},
                  makeNode("IndexExpr", {castNode("Expr", 0), castNode("Expr", 2)})}},

                // (EXPR) -> (LVALUE) [INCREMENT]
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::INCREMENT}},
                  makeNode("UnOpExpr", {firstNodePosition, "UnOpType::POST_INCREMENT", castNode("LValue", 0)})}},
                // (EXPR) -> (LVALUE) [DECREMENT]
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::DECREMENT}},
                  makeNode("UnOpExpr", {firstNodePosition, "UnOpType::POST_DECREMENT", castNode("LValue", 0)})}},
                // (EXPR) -> [INCREMENT] (LVALUE)
                {{{ASTNodeType::EXPR, {TokenType::INCREMENT, ASTNodeType::LVALUE}},
                  makeNode("UnOpExpr", {firstTokenPosition, "UnOpType::PRE_INCREMENT", castNode("LValue", 1)})}},
                // (EXPR) -> [DECREMENT] (LVALUE)
                {{{ASTNodeType::EXPR, {TokenType::DECREMENT, ASTNodeType::LVALUE}},
                  makeNode("UnOpExpr", {firstTokenPosition, "UnOpType::PRE_DECREMENT", castNode("LValue", 1)})}},

                // (EXPR) -> [NOT] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::NOT, ASTNodeType::EXPR}},
                  makeNode("UnOpExpr", {firstTokenPosition, "UnOpType::NOT", castNode("Expr", 1)})}},
                // (EXPR) -> [MINUS] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::MINUS, ASTNodeType::EXPR}},
                  makeNode("UnOpExpr", {firstTokenPosition, "UnOpType::NEGATE", castNode("Expr", 1)})}},

                // RA: (EXPR) -> (EXPR) [EXPONENTIATE] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::EXPONENTIATE, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EXPONENTIATE", castNode("Expr", 2)})}},

                // (Shared precedence)
                // LA: (EXPR) -> (EXPR) [MULTIPLY] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MULTIPLY, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MULTIPLY", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [DIVIDE] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DIVIDE, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::DIVIDE", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [MODULO] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MODULO, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MODULO", castNode("Expr", 2)})}},

                // (Shared precedence)
                // LA: (EXPR) -> (EXPR) [ADD] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::ADD, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::ADD", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [MINUS] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MINUS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::SUBTRACT", castNode("Expr", 2)})}},

                // (Shared precedence)
                // LA: (EXPR) -> (EXPR) [LESSER] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [LESSER_EQUAL] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER_EQUAL, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER_EQUAL", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [GREATER] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [GREATER_EQUAL] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER_EQUAL, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER_EQUAL", castNode("Expr", 2)})}},

                // (Shared precedence)
                // LA: (EXPR) -> (EXPR) [DOUBLE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DOUBLE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EQUAL", castNode("Expr", 2)})},
                 // LA: (EXPR) -> (EXPR) [NOT_EQUALS] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::NOT_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::NOT_EQUAL", castNode("Expr", 2)})}},

                // LA: (EXPR) -> (EXPR) [AND] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::AND, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::AND", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (EXPR) [OR] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::OR, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::OR", castNode("Expr", 2)})}},

                // (EXPR) -> [SCAN] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::SCAN, ASTNodeType::EXPR}},
                  makeNode("ScanExpr", {firstTokenPosition, castNode("Expr", 1)})}},
                // (EXPR) -> [SCAN] [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::SCAN, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  makeNode("ScanExpr", {firstTokenPosition, castNode("Expr", 2)})}},
                // (EXPR) -> [READ] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::READ, ASTNodeType::EXPR}},
                  makeNode("ReadExpr", {firstTokenPosition, castNode("Expr", 1)})}},
                // (EXPR) -> [READ] [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::READ, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  makeNode("ReadExpr", {firstTokenPosition, castNode("Expr", 2)})}},

                // (EXPR) -> [LSQUARE] [RSQUARE]
                {{{ASTNodeType::EXPR, {TokenType::LSQUARE, TokenType::RSQUARE}},
                  makeNode("ArrayExpr", {firstTokenPosition})}},
                // (EXPR) -> [LSQUARE] (EXPRS) [RSQUARE]
                {{{ASTNodeType::EXPR, {TokenType::LSQUARE, ASTNodeType::EXPRS, TokenType::RSQUARE}},
                  makeNode("ArrayExpr", {firstTokenPosition, castNode("Exprs", 1)})}},
                // (EXPR) -> [LSQUARE] (EXPRS) [COMMA] [RSQUARE]
                {{{ASTNodeType::EXPR, {TokenType::LSQUARE, ASTNodeType::EXPRS, TokenType::COMMA, TokenType::RSQUARE}},
                  makeNode("ArrayExpr", {firstTokenPosition, castNode("Exprs", 1)})}},

                // (EXPR) -> [LPAREN] [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::LPAREN, TokenType::RPAREN}},
                  makeNode("TupleExpr", {firstTokenPosition})}},
                // (EXPR) -> [LPAREN] (EXPRS) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPRS, TokenType::RPAREN}},
                  makeNode("TupleExpr", {firstTokenPosition, castNode("Exprs", 1)})}},
                // (EXPR) -> [LPAREN] (EXPRS) [COMMA] [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPRS, TokenType::COMMA, TokenType::RPAREN}},
                  makeNode("TupleExpr", {firstTokenPosition, castNode("Exprs", 1)})}},

                // LA: (EXPR) -> (LVALUE) [EXPONENTIATE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::EXPONENTIATE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::EXPONENTIATE_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [MULTIPLY_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::MULTIPLY_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::MULTIPLY_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [DIVIDE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::DIVIDE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::DIVIDE_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [MODULO_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::MODULO_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::MODULO_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [ADD_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::ADD_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::ADD_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [MINUS_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::MINUS_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::SUBTRACT_SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (LVALUE) [EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE, TokenType::EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("LValue", 0), "BinOpType::SET", castNode("Expr", 2)})}},
                // LA: (EXPR) -> (BINDING) [EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::BINDING, TokenType::EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Binding", 0), "BinOpType::SET", castNode("Expr", 2)})}},

                // (EXPR) -> (LVALUE)
                {{{ASTNodeType::EXPR, {ASTNodeType::LVALUE}}, makeNode("LValueExpr", {castNode("LValue", 0)})}},

                // (EXPR) -> [IF] (EXPR) [THEN] (EXPR) [ELSE] (EXPR)
                {{{ASTNodeType::EXPR,
                   {TokenType::IF, ASTNodeType::EXPR, TokenType::THEN, ASTNodeType::EXPR, TokenType::ELSE,
                    ASTNodeType::EXPR}},
                  makeNode("TernaryExpr",
                           {firstTokenPosition, castNode("Expr", 1), castNode("Expr", 3), castNode("Expr", 5)})}},


                // ==================
                // ||  Functions:  ||
                // ==================


                // (STMT) -> [FN] [VARIABLE] [LPAREN] [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FN, TokenType::VARIABLE, TokenType::LPAREN, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("FunctionStmt", {firstTokenPosition, tokenContents(1), castNode("Stmt", 4)})}},
                // (STMT) -> [FN] [VARIABLE] [LPAREN] (BINDINGS) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FN, TokenType::VARIABLE, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN,
                    ASTNodeType::STMT}},
                  makeNode("FunctionStmt",
                           {firstTokenPosition, tokenContents(1), castNode("Bindings", 3), castNode("Stmt", 5)})}},
                // (STMT) -> [FN] [VARIABLE] [LPAREN] [RPAREN] [COLON] (TYPE) (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FN, TokenType::VARIABLE, TokenType::LPAREN, TokenType::RPAREN, TokenType::COLON,
                    ASTNodeType::TYPE, ASTNodeType::STMT}},
                  makeNode("FunctionStmt",
                           {firstTokenPosition, tokenContents(1), castNode("Stmt", 6), castNode("Type", 5)})}},
                // (STMT) -> [FN] [VARIABLE] [LPAREN] (BINDINGS) [RPAREN] [COLON] (TYPE) (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FN, TokenType::VARIABLE, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN,
                    TokenType::COLON, ASTNodeType::TYPE, ASTNodeType::STMT}},
                  makeNode("FunctionStmt", {firstTokenPosition, tokenContents(1), castNode("Bindings", 3),
                                            castNode("Stmt", 7), castNode("Type", 6)})}},

                // (EXPR) -> [FN] [LPAREN] [RPAREN] [LAMBDA] (EXPR)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, TokenType::RPAREN, TokenType::LAMBDA, ASTNodeType::EXPR}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Expr", 4)})}},
                // (EXPR) -> [FN] [LPAREN] (BINDINGS) [RPAREN] [LAMBDA] (EXPR)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::LAMBDA,
                    ASTNodeType::EXPR}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Bindings", 2), castNode("Expr", 5)})}},
                // (EXPR) -> [FN] [LPAREN] [RPAREN] [COLON] (TYPE) [LAMBDA] (EXPR)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, TokenType::RPAREN, TokenType::COLON, ASTNodeType::TYPE,
                    TokenType::LAMBDA, ASTNodeType::EXPR}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Expr", 6), castNode("Type", 4)})}},
                // (EXPR) -> [FN] [LPAREN] (BINDINGS) [RPAREN] [COLON] (TYPE) [LAMBDA] (EXPR)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::COLON,
                    ASTNodeType::TYPE, TokenType::LAMBDA, ASTNodeType::EXPR}},
                  makeNode("LambdaExpr",
                           {firstTokenPosition, castNode("Bindings", 2), castNode("Expr", 7), castNode("Type", 5)})}},

                // (EXPR) -> [FN] [LPAREN] [RPAREN] [LAMBDA] (STMT)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, TokenType::RPAREN, TokenType::LAMBDA, ASTNodeType::STMT}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Stmt", 4)})}},
                // (EXPR) -> [FN] [LPAREN] (BINDINGS) [RPAREN] [LAMBDA] (STMT)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::LAMBDA,
                    ASTNodeType::STMT}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Bindings", 2), castNode("Stmt", 5)})}},
                // (EXPR) -> [FN] [LPAREN] [RPAREN] [COLON] (TYPE) [LAMBDA] (STMT)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, TokenType::RPAREN, TokenType::COLON, ASTNodeType::TYPE,
                    TokenType::LAMBDA, ASTNodeType::STMT}},
                  makeNode("LambdaExpr", {firstTokenPosition, castNode("Stmt", 6), castNode("Type", 4)})}},
                // (EXPR) -> [FN] [LPAREN] (BINDINGS) [RPAREN] [COLON] (TYPE) [LAMBDA] (STMT)
                {{{ASTNodeType::EXPR,
                   {TokenType::FN, TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::COLON,
                    ASTNodeType::TYPE, TokenType::LAMBDA, ASTNodeType::STMT}},
                  makeNode("LambdaExpr",
                           {firstTokenPosition, castNode("Bindings", 2), castNode("Stmt", 7), castNode("Type", 5)})}},


                // ===================
                // ||  Statements:  ||
                // ===================

                // (STMTS) -> (STMT)
                {{{ASTNodeType::STMTS, {ASTNodeType::STMT}}, makeNode("Stmts", {castNode("Stmt", 0)})}},
                // (STMTS) -> (STMTS) (STMT)
                {{{ASTNodeType::STMTS, {ASTNodeType::STMTS, ASTNodeType::STMT}},
                  makeNode("Stmts", {castNode("Stmts", 0), castNode("Stmt", 1)})}},

                // (STMT) -> [TIMEOUT] [INTVAL] (STRING) (STMT)
                {{{ASTNodeType::STMT, {TokenType::TIMEOUT, TokenType::INTVAL, ASTNodeType::STRING, ASTNodeType::STMT}},
                  makeNode("TimeoutStmt", {firstTokenPosition, callFunc("std::stoll", {tokenContents(1)}),
                                           castNode("String", 2), castNode("Stmt", 3)})}},
                // (STMT) -> [IMPORT] (STRING) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::IMPORT, ASTNodeType::STRING, TokenType::SEMICOLON}},
                  makeNode("ImportStmt", {firstTokenPosition, castNode("String", 1)})}},
                // (STMT) -> [ASSERT] (EXPR) [COMMA] (STRING) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::ASSERT, ASTNodeType::EXPR, TokenType::COMMA, ASTNodeType::STRING, TokenType::SEMICOLON}},
                  makeNode("AssertStmt", {firstTokenPosition, castNode("Expr", 1), castNode("String", 3)})}},

                // (STMT) -> [RETURN] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::RETURN, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("ReturnStmt", {firstTokenPosition, castNode("Expr", 1)})}},

                // (STMT) -> [PRINT] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::PRINT, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("PrintStmt", {firstTokenPosition, castNode("Expr", 1)})}},
                // (STMT) -> [PRINT] [LPAREN] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::PRINT, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("PrintStmt", {firstTokenPosition, castNode("Expr", 2)})}},
                // (STMT) -> [PRINTLN] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::PRINTLN, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("PrintlnStmt", {firstTokenPosition, castNode("Expr", 1)})}},
                // (STMT) -> [PRINTLN] [LPAREN] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::PRINTLN, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("PrintlnStmt", {firstTokenPosition, castNode("Expr", 2)})}},

                // (STMT) -> [WRITE] (EXPR) [TO] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::WRITE, ASTNodeType::EXPR, TokenType::TO, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("WriteStmt", {firstTokenPosition, castNode("Expr", 1), castNode("Expr", 3)})}},
                // (STMT) -> [WRITE] [LPAREN] (EXPR) [COMMA] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::WRITE, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::COMMA, ASTNodeType::EXPR,
                    TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("WriteStmt", {firstTokenPosition, castNode("Expr", 2), castNode("Expr", 4)})}},

                // (STMT) -> [ALIAS] [VARIABLE] [EQUALS] (ASYNC_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::ALIAS, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::ASYNC_CMD,
                    TokenType::SEMICOLON}},
                  makeNode("AliasStmt", {firstTokenPosition, tokenContents(1), castNode("AsyncCmd", 3)})}},
                // (STMT) -> [ALIAS] [VARIABLE] [EQUALS] (PIPE_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::ALIAS, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::PIPE_CMD,
                    TokenType::SEMICOLON}},
                  makeNode("AliasStmt", {firstTokenPosition, tokenContents(1), castNode("PipeCmd", 3)})}},
                // (STMT) -> [ALIAS] [VARIABLE] [EQUALS] (BASIC_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::ALIAS, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::BASIC_CMD,
                    TokenType::SEMICOLON}},
                  makeNode("AliasStmt", {firstTokenPosition, tokenContents(1), castNode("BasicCmd", 3)})}},

                // (STMT) -> [TYPE] [VARIABLE] [EQUALS] (TYPE) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::TYPE, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::TYPE, TokenType::SEMICOLON}},
                  makeNode("TypeStmt", {firstTokenPosition, tokenContents(1), castNode("Type", 3)})}},

                // LA: (STMT) -> [IF] [LPAREN] (EXPR) [RPAREN] (STMT) [ELSE] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::IF, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT,
                    TokenType::ELSE, ASTNodeType::STMT}},
                  makeNode("IfStmt",
                           {firstTokenPosition, castNode("Expr", 2), castNode("Stmt", 4), castNode("Stmt", 6)})}},
                // (STMT) -> [IF] [LPAREN] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::IF, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("IfStmt", {firstTokenPosition, castNode("Expr", 2), castNode("Stmt", 4)})}},

                // (STMT) -> [FOR] [LPAREN] [SEMICOLON] [SEMICOLON] [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, TokenType::SEMICOLON, TokenType::SEMICOLON, TokenType::RPAREN,
                    ASTNodeType::STMT}},
                  // Note: while statement is used here for simplicity, no need to use a for loop for this case
                  makeNode("WhileStmt", {firstTokenPosition, makeNode("BoolExpr", {tokenPosition(3), "true"}),
                                         castNode("Stmt", 5)})}},
                // (STMT) -> [FOR] [LPAREN] (EXPR) [SEMICOLON] [SEMICOLON] [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::SEMICOLON, TokenType::SEMICOLON,
                    TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt",
                           {firstTokenPosition, castNode("Stmt", 2), makeNode("BoolExpr", {tokenPosition(4), "true"}),
                            "nullptr", castNode("Stmt", 6)})}},
                // (STMT) -> [FOR] [LPAREN] [SEMICOLON] (EXPR) [SEMICOLON] [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, TokenType::SEMICOLON, ASTNodeType::EXPR, TokenType::SEMICOLON,
                    TokenType::RPAREN, ASTNodeType::STMT}},
                  // Note: while statement is used here for simplicity, no need to use a for loop for this case
                  makeNode("WhileStmt", {firstTokenPosition, castNode("Expr", 3), castNode("Stmt", 6)})}},
                // (STMT) -> [FOR] [LPAREN] [SEMICOLON] [SEMICOLON] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, TokenType::SEMICOLON, TokenType::SEMICOLON, ASTNodeType::EXPR,
                    TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt", {firstTokenPosition, "nullptr", makeNode("BoolExpr", {tokenPosition(3), "true"}),
                                       castNode("Stmt", 4), castNode("Stmt", 6)})}},
                // (STMT) -> [FOR] [LPAREN] [SEMICOLON] (EXPR) [SEMICOLON] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, TokenType::SEMICOLON, ASTNodeType::EXPR, TokenType::SEMICOLON,
                    ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt", {firstTokenPosition, "nullptr", castNode("Expr", 3), castNode("Stmt", 5),
                                       castNode("Stmt", 7)})}},
                // (STMT) -> [FOR] [LPAREN] (EXPR) [SEMICOLON] [SEMICOLON] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::SEMICOLON, TokenType::SEMICOLON,
                    ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt",
                           {firstTokenPosition, castNode("Stmt", 2), makeNode("BoolExpr", {tokenPosition(4), "true"}),
                            castNode("Stmt", 5), castNode("Stmt", 7)})}},
                // (STMT) -> [FOR] [LPAREN] (EXPR) [SEMICOLON] (EXPR) [SEMICOLON] [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::SEMICOLON, ASTNodeType::EXPR,
                    TokenType::SEMICOLON, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt", {firstTokenPosition, castNode("Stmt", 2), castNode("Expr", 4), "nullptr",
                                       castNode("Stmt", 7)})}},
                // (STMT) -> [FOR] [LPAREN] (EXPR) [SEMICOLON] (EXPR) [SEMICOLON] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::SEMICOLON, ASTNodeType::EXPR,
                    TokenType::SEMICOLON, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt", {firstTokenPosition, castNode("Stmt", 2), castNode("Expr", 4),
                                       castNode("Stmt", 6), castNode("Stmt", 8)})}},
                // (STMT) -> [WHILE] [LPAREN] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("WhileStmt", {firstTokenPosition, castNode("Expr", 2), castNode("Stmt", 4)})}},
                // (STMT) -> [DO] (STMT) [WHILE] [LPAREN] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::DO, ASTNodeType::STMT, TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR,
                    TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("DoWhileStmt", {firstTokenPosition, castNode("Expr", 4), castNode("Stmt", 1)})}},

                // (STMT) -> [BREAK] [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::BREAK, TokenType::SEMICOLON}},
                  makeNode("BreakStmt", {firstTokenPosition})}},
                // (STMT) -> [CONTINUE] [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::CONTINUE, TokenType::SEMICOLON}},
                  makeNode("ContinueStmt", {firstTokenPosition})}},

                // (STMT) -> (ASYNC_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::ASYNC_CMD, TokenType::SEMICOLON}},
                  makeNode("CmdStmt", {castNode("AsyncCmd", 0)})}},
                // (STMT) -> (PIPE_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::PIPE_CMD, TokenType::SEMICOLON}},
                  makeNode("CmdStmt", {castNode("PipeCmd", 0)})}},
                // (STMT) -> (BASIC_CMD) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::BASIC_CMD, TokenType::SEMICOLON}},
                  makeNode("CmdStmt", {castNode("BasicCmd", 0)})}},
                // (STMT) -> (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("ExprStmt", {castNode("Expr", 0)})}},


                // ================
                // ||  Strings:  ||
                // ================

                // (STRING_EXPRS) -> [STRINGLITERAL]
                {{{ASTNodeType::STRING_EXPRS, {TokenType::STRINGLITERAL}},
                  makeNode("StringExprs", {firstTokenPosition, tokenContents(0)})}},
                // (STRING_EXPRS) -> (STRING_EXPRS) (EXPR) [STRINGLITERAL]
                {{{ASTNodeType::STRING_EXPRS, {ASTNodeType::STRING_EXPRS, ASTNodeType::EXPR, TokenType::STRINGLITERAL}},
                  makeNode("StringExprs",
                           {tokenPosition(2), tokenContents(2), castNode("Expr", 1), castNode("StringExprs", 0)})}},
                // (STRING) -> [STRINGVAL] (STRING_EXPRS) [ENDSTRINGVAL]
                {{{ASTNodeType::STRING, {TokenType::STRINGVAL, ASTNodeType::STRING_EXPRS, TokenType::ENDSTRINGVAL}},
                  makeNode("String", {firstTokenPosition, castNode("StringExprs", 1)})}},

                // ==============
                // ||  Types:  ||
                // ==============

                // (TYPE) -> [LPAREN] (TYPE) [RPAREN]
                {{{ASTNodeType::TYPE, {TokenType::LPAREN, ASTNodeType::TYPE, TokenType::RPAREN}},
                  "productionList[1].node"}},

                // (TYPES) -> (TYPE)
                {{{ASTNodeType::TYPES, {ASTNodeType::TYPE}}, makeNode("Types", {castNode("Type", 0)})}},
                // (TYPES) -> (TYPES) [COMMA] (TYPE)
                {{{ASTNodeType::TYPES, {ASTNodeType::TYPES, TokenType::COMMA, ASTNodeType::EXPR}},
                  makeNode("Types", {castNode("Types", 0), castNode("Type", 2)})}},

                // (TYPE) -> [INT]
                {{{ASTNodeType::TYPE, {TokenType::INT}}, makeNode("IntType", {firstTokenPosition})}},
                // (TYPE) -> [FLOAT]
                {{{ASTNodeType::TYPE, {TokenType::FLOAT}}, makeNode("FloatType", {firstTokenPosition})}},
                // (TYPE) -> [BOOL]
                {{{ASTNodeType::TYPE, {TokenType::BOOL}}, makeNode("BoolType", {firstTokenPosition})}},
                // (TYPE) -> [STRING]
                {{{ASTNodeType::TYPE, {TokenType::STRING}}, makeNode("StringType", {firstTokenPosition})}},
                // (TYPE) -> [VOID]
                {{{ASTNodeType::TYPE, {TokenType::VOID}},
                  makeNode("TupleType", {firstTokenPosition, makeNode("Types", {firstTokenPosition})})}},
                // (TYPE) -> [VARIABLE]
                {{{ASTNodeType::TYPE, {TokenType::VARIABLE}},
                  makeNode("VariableType", {firstTokenPosition, tokenContents(0)})}},

                // (TYPE) -> [LPAREN] [RPAREN] [LAMBDA] (TYPE)
                {{{ASTNodeType::TYPE, {TokenType::LPAREN, TokenType::RPAREN, TokenType::LAMBDA, ASTNodeType::TYPE}},
                  makeNode("FunctionType", {firstTokenPosition, castNode("Type", 3)})}},
                // (TYPE) -> [LPAREN] (TYPES) [RPAREN] [LAMBDA] (TYPE)
                {{{ASTNodeType::TYPE,
                   {TokenType::LPAREN, ASTNodeType::TYPES, TokenType::RPAREN, TokenType::LAMBDA, ASTNodeType::TYPE}},
                  makeNode("FunctionType", {firstTokenPosition, castNode("Types", 1), castNode("Type", 4)})}},

                // (TYPE) -> (TYPE) [LSQUARE] [RSQUARE]
                {{{ASTNodeType::TYPE, {ASTNodeType::TYPE, TokenType::LSQUARE, TokenType::RSQUARE}},
                  makeNode("ArrayType", {castNode("Type", 0)})}},

                // (TYPE) -> [LPAREN] [RPAREN]
                {{{ASTNodeType::TYPE, {TokenType::LPAREN, TokenType::RPAREN}},
                  makeNode("TupleType", {firstTokenPosition})}},
                // (TYPE) -> [LPAREN] (TYPES) [RPAREN]
                {{{ASTNodeType::TYPE, {TokenType::LPAREN, ASTNodeType::TYPES, TokenType::RPAREN}},
                  makeNode("TupleType", {firstTokenPosition, castNode("Types", 1)})}},
                // (TYPE) -> [LPAREN] (TYPES) [COMMA] [RPAREN]
                {{{ASTNodeType::TYPE, {TokenType::LPAREN, ASTNodeType::TYPES, TokenType::COMMA, TokenType::RPAREN}},
                  makeNode("TupleType", {firstTokenPosition, castNode("Types", 1)})}},
        });
    }
#pragma clang diagnostic pop
}  // namespace Parser

namespace std {
    std::size_t hash<Parser::GrammarEntry>::operator()(const Parser::GrammarEntry& grammarEntry) const noexcept {
        if (grammarEntry.grammarEntryType == Parser::GrammarEntry::TOKEN_TYPE) {
            return Util::combineHashes(grammarEntry.grammarEntryType, grammarEntry.tokenType);
        }
        return Util::combineHashes(grammarEntry.grammarEntryType, grammarEntry.nodeType);
    }

    std::size_t hash<Parser::GrammarRule>::operator()(const Parser::GrammarRule& grammarRule) const noexcept {
        constexpr hash<Parser::GrammarEntry> hashEntry;
        vector<std::size_t> hashVals(grammarRule.components.size() + 1);
        for (std::size_t ind = 0; ind < grammarRule.components.size(); ++ind) {
            hashVals[ind] = hashEntry(grammarRule.components[ind]);
        }
        hashVals.back() = hashEntry(grammarRule.result);
        return Util::combineHashes(hashVals);
    }
}  // namespace std
