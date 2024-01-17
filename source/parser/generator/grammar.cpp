/**
 * @file source/parser/generator/grammar.hpp
 * @brief Contains the definition of the Commander grammar.
 */

#include "grammar.hpp"

#include "source/util/combine_hashes.hpp"

namespace Parser {

    Grammar::GrammarEntry::GrammarEntry(Grammar::TokenType tokenType) : tokenType(tokenType) {}

    Grammar::GrammarEntry::GrammarEntry(ASTNodeType nodeType) : nodeType(nodeType) {}

    size_t Grammar::GrammarEntry::Hash::operator()(const Grammar::GrammarEntry& entry) const {
        const std::hash<size_t> hash;

        size_t val = 0;
        if (entry.tokenType.has_value()) val = Util::combineHashes(val, hash(entry.tokenType.value()));
        if (entry.nodeType.has_value()) val = Util::combineHashes(val, hash(entry.nodeType.value()));

        return val;
    }

    bool Grammar::GrammarEntry::operator==(const Grammar::GrammarEntry& other) const {
        if (this->tokenType.has_value()) {
            if (!other.tokenType.has_value()) return false;
            if (this->tokenType.value() != other.tokenType.value()) return false;
        } else {
            if (other.tokenType.has_value()) return false;
        }

        if (this->nodeType.has_value()) {
            if (!other.nodeType.has_value()) return false;
            if (this->nodeType.value() != other.nodeType.value()) return false;
        } else {
            if (other.nodeType.has_value()) return false;
        }

        return true;
    }

    bool Grammar::GrammarEntry::operator!=(const Grammar::GrammarEntry& other) const { return !(*this == other); }

    bool Grammar::GrammarRule::operator==(const Grammar::GrammarRule& other) const {
        if (this->result != other.result) return false;

        if (this->components.size() != other.components.size()) return false;

        for (size_t ind = 0; ind < this->components.size(); ++ind) {
            if (this->components[ind] != other.components[ind]) return false;
        }

        return true;
    }

    bool Grammar::GrammarRule::operator!=(const Grammar::GrammarRule& other) const { return !(*this == other); }

    size_t Grammar::GrammarRule::Hash::operator()(const Grammar::GrammarRule& rule) const {
        const GrammarEntry::Hash hashEntry;

        size_t val = std::hash<size_t> {}(rule.result);
        for (const auto& entry : rule.components) val = Util::combineHashes(val, hashEntry(entry));

        return val;
    }


    Grammar::Grammar() : Grammar(_defineGrammar()) {}

    Grammar::Grammar(const std::vector<std::tuple<GrammarRule, NodeConstructor>>& grammarDefinitions)
        : rules([&]() {
              std::vector<GrammarRule> result(grammarDefinitions.size());
              size_t index = 0;
              for (const auto& item : grammarDefinitions) result[index++] = std::get<0>(item);
              return result;
          }()),
          reductions([&]() {
              std::unordered_map<GrammarRule, NodeConstructor, GrammarRule::Hash> result;
              for (const auto& item : grammarDefinitions) result[std::get<0>(item)] = std::get<1>(item);
              return result;
          }()) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
    std::vector<std::tuple<Grammar::GrammarRule, Grammar::NodeConstructor>> Grammar::_defineGrammar() {
        const auto fold = [](const std::vector<std::string>& strings) -> std::string {
            if (strings.empty()) return "";
            const std::string delim = ", ";
            std::string result;
            for (size_t index = 0; index < strings.size() - 1; ++index) result += strings[index] + delim;
            result += strings[strings.size() - 1];
            return result;
        };
        const auto callFunc = [&](const std::string& func, const std::vector<std::string>& args) -> std::string {
            return func + "(" + fold(args) + ")";
        };
        const auto makeNode = [&](const std::string& nodeType, const std::vector<std::string>& args) -> std::string {
            return callFunc("std::make_shared<" + nodeType + "Node>", args);
        };
        const auto castNode = [&](const std::string& nodeType, size_t index) -> std::string {
            return callFunc("cast" + nodeType, {"productionList[" + std::to_string(index) + "].node"});
        };
        const auto tokenContents = [](size_t index) -> std::string {
            return "productionList[" + std::to_string(index) + "].token->contents";
        };
        return {//  PRGM -> STMT
                {{ASTNodeType::PRGM, {ASTNodeType::STMT}},
                 makeNode("Prgm", {"std::vector<StmtNodePtr> {" + castNode("Stmt", 0) + "}"})},

                //  STMT -> EXPR "[SEMICOLON]"
                {{ASTNodeType::STMT, {ASTNodeType::EXPR, TokenType::SEMICOLON}},
                 makeNode("ExprStmt", {castNode("Expr", 0)})},

                //  EXPR -> "[INTVAL]"
                {{ASTNodeType::EXPR, {TokenType::INTVAL}},
                 makeNode("IntExpr", {callFunc("std::stoll", {tokenContents(0)})})},
                //  EXPR -> "[FLOATVAL]"
                {{ASTNodeType::EXPR, {TokenType::FLOATVAL}},
                 makeNode("FloatExpr", {callFunc("std::stold", {tokenContents(0)})})},
                //  EXPR -> "[TRUE]"
                {{ASTNodeType::EXPR, {TokenType::TRUE}}, makeNode("BoolExpr", {"true"})},
                //  EXPR -> "[FALSE]"
                {{ASTNodeType::EXPR, {TokenType::FALSE}}, makeNode("BoolExpr", {"false"})},
                //  EXPR -> EXPR "[QUESTION]" EXPR "[COLON]" EXPR
                {{ASTNodeType::EXPR,
                  {ASTNodeType::EXPR, TokenType::QUESTION, ASTNodeType::EXPR, TokenType::COLON, ASTNodeType::EXPR}},
                 makeNode("TernaryExpr", {castNode("Expr", 0), castNode("Expr", 2), castNode("Expr", 4)})},
                //  EXPR -> EXPR "[EXPONENTIATE]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::EXPONENTIATE, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EXPONENTIATE", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[MULTIPLY]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MULTIPLY, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MULTIPLY", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[DIVIDE]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DIVIDE, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::DIVIDE", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[MODULO]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MODULO, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::MODULO", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[ADD]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::ADD, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::ADD", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[MINUS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MINUS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::SUBTRACT", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[LESSER]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[LESSER_EQUAL]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER_EQUAL, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::LESSER_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[GREATER]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[GREATER_EQUAL]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER_EQUAL, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::GREATER_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[DOUBLE_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DOUBLE_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::EQUAL", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[NOT_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::NOT_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::NOT_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[AND]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::AND, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::AND", castNode("Expr", 2)})},
                //  EXPR -> EXPR "[OR]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::OR, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Expr", 0), "BinOpType::OR", castNode("Expr", 2)})},

                //  EXPR -> VARIABLE "[EXPONENTIATE_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EXPONENTIATE_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr",
                          {castNode("Variable", 0), "BinOpType::EXPONENTIATE_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[MULTIPLY_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MULTIPLY_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::MULTIPLY_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[DIVIDE_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::DIVIDE_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::DIVIDE_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[MODULO_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MODULO_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::MODULO_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[ADD_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::ADD_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::ADD_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[MINUS_EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MINUS_EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::SUBTRACT_EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE "[EQUALS]" EXPR
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EQUALS, ASTNodeType::EXPR}},
                 makeNode("BinOpExpr", {castNode("Variable", 0), "BinOpType::EQUAL", castNode("Expr", 2)})},
                //  EXPR -> VARIABLE
                {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE}}, makeNode("VarExpr", {castNode("Variable", 0)})},
                //  EXPR -> "[(]" EXPR "[)]"
                {{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                 "productionList[1].node"},

                //  VARIABLE -> "[VARIABLE]"
                {{ASTNodeType::VARIABLE, {TokenType::VARIABLE}}, makeNode("IdentVariable", {tokenContents(0)})}};
    }
#pragma clang diagnostic pop

}  //  namespace Parser
