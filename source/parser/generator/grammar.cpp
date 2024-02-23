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

//  ReSharper disable CppRedundantQualifier

namespace Parser {

    GrammarEntry::GrammarEntry(const Grammar::TokenType tokenType)
        : grammarEntryType(TOKEN_TYPE), tokenType(tokenType), nodeType() {}

    GrammarEntry::GrammarEntry(const ASTNodeType nodeType)
        : grammarEntryType(NODE_TYPE), tokenType(), nodeType(nodeType) {}

    bool GrammarEntry::operator==(const GrammarEntry& other) const {
        constexpr std::hash<GrammarEntry> hash;
        return hash(*this) == hash(other);
    }

    bool GrammarEntry::operator!=(const GrammarEntry& other) const { return !(*this == other); }

    std::ostream& operator<<(std::ostream& stream, const GrammarEntry& grammarEntry) {
        if (grammarEntry.grammarEntryType == GrammarEntry::TOKEN_TYPE) {
            stream << "[" << tokenTypeToString(grammarEntry.tokenType) << "]";
        } else {
            stream << "(" << nodeTypeToString(grammarEntry.nodeType) << ")";
        }
        return stream;
    }

    bool GrammarRule::operator==(const GrammarRule& other) const {
        if (this->result != other.result) return false;

        if (this->components.size() != other.components.size()) return false;

        for (std::size_t ind = 0; ind < this->components.size(); ++ind) {
            if (this->components[ind] != other.components[ind]) return false;
        }

        return true;
    }

    bool GrammarRule::operator!=(const GrammarRule& other) const { return !(*this == other); }

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
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
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
        const auto stringTokenContents = [&](const std::size_t index) -> std::string {
            //  FIXME: Doesn't support string interpolation or empty strings.
            return callFunc("std::reinterpret_pointer_cast<Lexer::StringToken>",
                            {"productionList[" + std::to_string(index) + "].token"})
                 + "->subTokens[0]->contents";
        };
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
                //  ================
                //  ||  Program:  ||
                //  ================

                //  (PRGM) -> (STMTS)
                {{{ASTNodeType::PRGM, {ASTNodeType::STMTS}}, makeNode("Prgm", {castNode("Stmts", 0) + "->stmts"})}},


                //  =================
                //  ||  Bindings:  ||
                //  =================

                //  (BINDINGS) -> (BINDING)
                //  TODO
                //  (BINDINGS) -> (BINDINGS) [COMMA] (BINDING)
                //  TODO

                //  (BINDING) -> [VARIABLE]
                //  TODO
                //  (BINDING) -> [VARIABLE] [COLON] (TYPE)
                //  TODO


                //  =================
                //  ||  Commands:  ||
                //  =================

                //  (CMD) -> (CMD) [AMPERSAND]
                {{{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::AMPERSAND}},
                  makeNode("AsyncCmd", {castNode("Cmd", 0)})}},
                //  (CMD) -> (CMD) [PIPE] (CMD)
                {{{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::PIPE, ASTNodeType::CMD}},
                  makeNode("PipeCmd", {castNode("Cmd", 0), castNode("Cmd", 2)})}},

                //  (CMD) -> [CMDSTRINGVAL]
                {{{ASTNodeType::CMD, {TokenType::CMDSTRINGVAL}},
                  makeNode("CmdCmd", {makeNode("String", {tokenContents(0)})})}},
                //  (CMD) -> (STRING)
                {{{ASTNodeType::CMD, {ASTNodeType::STRING}}, makeNode("CmdCmd", {castNode("String", 0)})}},
                //  (CMD) -> [VARIABLE]
                //  TODO
                //  (CMD) -> (CMD) [CMDSTRINGVAL]
                {{{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::CMDSTRINGVAL}},
                  makeNode("CmdCmd", {castNode("Cmd", 0), makeNode("String", {tokenContents(1)})})}},
                //  (CMD) -> (CMD) (STRING)
                {{{ASTNodeType::CMD, {ASTNodeType::CMD, ASTNodeType::STRING}},
                  makeNode("CmdCmd", {castNode("Cmd", 0), castNode("String", 1)})}},
                //  (CMD) -> (CMD) [VARIABLE]
                //  TODO


                //  ====================
                //  ||  Expressions:  ||
                //  ====================

                //  (EXPRS) -> (EXPR)
                //  TODO
                //  (EXPRS) -> (EXPRS) [COMMA] (EXPR)
                //  TODO

                //  Literals:
                //  ---------
                //  (EXPR) -> [INTVAL]
                {{{ASTNodeType::EXPR, {TokenType::INTVAL}},
                  makeNode("IntExpr", {callFunc("std::stoll", {tokenContents(0)})})}},
                //  (EXPR) -> [FLOATVAL]
                {{{ASTNodeType::EXPR, {TokenType::FLOATVAL}},
                  makeNode("FloatExpr", {callFunc("std::stold", {tokenContents(0)})})}},
                //  (EXPR) -> [TRUE]
                {{{ASTNodeType::EXPR, {TokenType::TRUE}}, makeNode("BoolExpr", {"true"})}},
                //  (EXPR) -> [FALSE]
                {{{ASTNodeType::EXPR, {TokenType::FALSE}}, makeNode("BoolExpr", {"false"})}},

                //  Binary operations:
                //  ------------------
                //  (EXPR) -> (EXPR) [EXPONENTIATE] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::EXPONENTIATE, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EXPONENTIATE", castNode("Expr", 2)})}},

                //  (Shared precedence)
                {//  (EXPR) -> (EXPR) [MULTIPLY] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MULTIPLY, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MULTIPLY", castNode("Expr", 2)})},
                 //  (EXPR) -> (EXPR) [DIVIDE] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DIVIDE, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::DIVIDE", castNode("Expr", 2)})},
                 //  (EXPR) -> (EXPR) [MODULO] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MODULO, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MODULO", castNode("Expr", 2)})}},

                //  (Shared precedence)
                //  (EXPR) -> (EXPR) [ADD] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::ADD, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::ADD", castNode("Expr", 2)})},
                 //  (EXPR) -> (EXPR) [MINUS] (EXPR)
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MINUS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::SUBTRACT", castNode("Expr", 2)})}},

                //  Comparison operations:
                //  ----------------------
                //  (EXPR) -> (EXPR) [LESSER] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [LESSER_EQUAL] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER_EQUAL, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER_EQUAL", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [GREATER] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [GREATER_EQUAL] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER_EQUAL, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER_EQUAL", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [DOUBLE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DOUBLE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EQUAL", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [NOT_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::NOT_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::NOT_EQUAL", castNode("Expr", 2)})}},

                //  Boolean operations:
                //  -------------------
                //  (EXPR) -> (EXPR) [AND] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::AND, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::AND", castNode("Expr", 2)})}},
                //  (EXPR) -> (EXPR) [OR] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::OR, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::OR", castNode("Expr", 2)})}},

                //  Ternary:
                //  --------
                //  (EXPR) -> (EXPR) [QUESTION] (EXPR) [COLON] (EXPR)
                {{{ASTNodeType::EXPR,
                   {ASTNodeType::EXPR, TokenType::QUESTION, ASTNodeType::EXPR, TokenType::COLON, ASTNodeType::EXPR}},
                  makeNode("TernaryExpr", {castNode("Expr", 0), castNode("Expr", 2), castNode("Expr", 4)})}},

                //  Variable set:
                //  -------------
                //  (EXPR) -> (VARIABLE) [EXPONENTIATE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EXPONENTIATE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr",
                           {castNode("Variable", 0), "BinOpType::EXPONENTIATE_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [MULTIPLY_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MULTIPLY_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::MULTIPLY_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [DIVIDE_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::DIVIDE_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::DIVIDE_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [MODULO_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MODULO_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::MODULO_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [ADD_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::ADD_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::ADD_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [MINUS_EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MINUS_EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::SUBTRACT_SET", castNode("Expr", 2)})}},
                //  (EXPR) -> (VARIABLE) [EQUALS] (EXPR)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EQUALS, ASTNodeType::EXPR}},
                  makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::SET", castNode("Expr", 2)})}},

                //  Parentheses:
                //  ------------
                //  (EXPR) -> [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  "productionList[1].node"}},

                //  Other:
                //  ------
                //  (EXPR) -> (VARIABLE)
                {{{ASTNodeType::EXPR, {ASTNodeType::VARIABLE}}, makeNode("VarExpr", {castNode("Variable", 0)})}},
                //  (EXPR) -> (STRING)
                {{{ASTNodeType::EXPR, {ASTNodeType::STRING}}, makeNode("StringExpr", {castNode("String", 0)})}},
                //  (EXPR) -> [SCAN] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::SCAN, ASTNodeType::EXPR}},
                  makeNode("ScanExpr", {castNode("Expr", 1)})}},
                //  (EXPR) -> [SCAN] [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::SCAN, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  makeNode("ScanExpr", {castNode("Expr", 2)})}},
                //  (EXPR) -> [READ] (EXPR)
                {{{ASTNodeType::EXPR, {TokenType::READ, ASTNodeType::EXPR}},
                  makeNode("ReadExpr", {castNode("Expr", 1)})}},
                //  (EXPR) -> [READ] [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::EXPR, {TokenType::READ, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  makeNode("ReadExpr", {castNode("Expr", 2)})}},

                //  Functions:
                //  ----------
                //  (EXPR) -> (EXPR) (LPAREN) (EXPRS) (RPAREN)
                //  TODO

                //  ===================
                //  ||  Statements:  ||
                //  ===================

                //  (STMTS) -> (STMT)
                {{{ASTNodeType::STMTS, {ASTNodeType::STMT}}, makeNode("Stmts", {castNode("Stmt", 0)})}},
                //  (STMTS) -> (STMTS) (STMT)
                {{{ASTNodeType::STMTS, {ASTNodeType::STMTS, ASTNodeType::STMT}},
                  makeNode("Stmts", {castNode("Stmts", 0) + "->stmts", castNode("Stmt", 1)})}},

                //  (STMT) -> [IMPORT] (EXPR)
                {{{ASTNodeType::STMT, {TokenType::IMPORT, ASTNodeType::EXPR}},
                  makeNode("ImportStmt", {castNode("Expr", 1)})}},

                //  (STMT) -> [PRINT] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::PRINT, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("PrintStmt", {castNode("Expr", 1)})}},
                //  (STMT) -> [PRINT] [LPAREN] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::PRINT, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("PrintStmt", {castNode("Expr", 2)})}},
                //  (STMT) -> [PRINTLN] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {TokenType::PRINTLN, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("PrintlnStmt", {castNode("Expr", 1)})}},
                //  (STMT) -> [PRINTLN] [LPAREN] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::PRINTLN, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("PrintlnStmt", {castNode("Expr", 2)})}},
                //  (STMT) -> [WRITE] (EXPR) [TO] (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::WRITE, ASTNodeType::EXPR, TokenType::TO, ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("WriteStmt", {castNode("Expr", 1), castNode("Expr", 3)})}},
                //  (STMT) -> [WRITE] [LPAREN] (EXPR) [COMMA] (EXPR) [RPAREN] [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::READ, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::COMMA, ASTNodeType::EXPR,
                    TokenType::RPAREN, TokenType::SEMICOLON}},
                  makeNode("WriteStmt", {castNode("Expr", 2), castNode("Expr", 4)})}},

                //  (STMT) -> [ALIAS] [VARIABLE] [EQUALS] (CMD) [SEMICOLON]
                {{{ASTNodeType::STMT,
                   {TokenType::ALIAS, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::CMD, TokenType::SEMICOLON}},
                  makeNode("AliasStmt", {tokenContents(1), castNode("Cmd", 3)})}},

                //  (STMT) -> [IF] [LPAREN] (EXPR) [RPAREN] (STMT) [ELSE] (STMT)
                //  TODO
                //  (STMT) -> [IF] [LPAREN] (EXPR) [RPAREN] (STMT)
                //  TODO

                //  (STMT) -> [FOR] [LPAREN] (EXPR) [SEMICOLON] (EXPR) [SEMICOLON] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::SEMICOLON, ASTNodeType::EXPR,
                    TokenType::SEMICOLON, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("ForStmt",
                           {castNode("Stmt", 2), castNode("Expr", 4), castNode("Stmt", 6), castNode("Stmt", 8)})}},

                //  (STMT) -> [WHILE] [LPAREN] (EXPR) [RPAREN] (STMT)
                {{{ASTNodeType::STMT,
                   {TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  makeNode("WhileStmt", {castNode("Expr", 2), castNode("Stmt", 4)})}},
                //  (STMT) -> [DO] (STMT) [WHILE] [LPAREN] (EXPR) [RPAREN]
                {{{ASTNodeType::STMT,
                   {TokenType::DO, ASTNodeType::STMT, TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR,
                    TokenType::RPAREN}},
                  makeNode("DoWhileStmt", {castNode("Expr", 4), castNode("Stmt", 1)})}},

                //  (STMT) -> [LCURLY] (STMTS) [RCURLY]
                //  TODO

                //  Functions:
                //  ----------
                //  (STMT) -> (VARIABLE) [LPAREN] (BINDINGS) [RPAREN] [LCURLY] (STMTS) [RCURLY]
                //  TODO
                //  (STMT) -> (VARIABLE) [LPAREN] (BINDINGS) [RPAREN] [COLON] (TYPE) [LCURLY] (STMTS) [RCURLY]
                //  TODO

                //  (STMT) -> (CMD) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::CMD, TokenType::SEMICOLON}},
                  makeNode("CmdStmt", {castNode("Cmd", 0)})}},
                //  (STMT) -> (EXPR) [SEMICOLON]
                {{{ASTNodeType::STMT, {ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  makeNode("ExprStmt", {castNode("Expr", 0)})}},


                //  ================
                //  ||  Strings:  ||
                //  ================

                //  (STRING) -> [STRINGVAL]
                {{{ASTNodeType::STRING, {TokenType::STRINGVAL}}, makeNode("String", {stringTokenContents(0)})}},


                //  ==============
                //  ||  Types:  ||
                //  ==============

                //  (TYPE) -> [INT]
                //  TODO
                //  (TYPE) -> [FLOAT]
                //  TODO


                //  ==================
                //  ||  Variables:  ||
                //  ==================

                //  (VARIABLE) -> [VARIABLE]
                {{{ASTNodeType::VARIABLE, {TokenType::VARIABLE}}, makeNode("IdentVariable", {tokenContents(0)})}},
        });
    }
#pragma clang diagnostic pop

}  //  namespace Parser

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

}  //  namespace std
