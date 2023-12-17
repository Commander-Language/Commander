/**
 * @file grammar.hpp
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


    Grammar::Grammar() : Grammar(_definePrototypeGrammar()) {}

    Grammar::Grammar(const std::vector<std::tuple<GrammarRule, NodeConstructor>>& grammarDefinitions)
        : rules([&]() {
              std::vector<GrammarRule> result;
              for (const auto& item : grammarDefinitions) result.emplace_back(std::get<0>(item));
              return result;
          }()),
          reductions([&]() {
              std::unordered_map<GrammarRule, NodeConstructor, GrammarRule::Hash> result;
              for (const auto& item : grammarDefinitions) result[std::get<0>(item)] = std::get<1>(item);
              return result;
          }()) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
    std::vector<std::tuple<Grammar::GrammarRule, Grammar::NodeConstructor>> Grammar::_definePrototypeGrammar() {
        //  Some shorthands for `std::reinterpret_pointer_cast`.
        //  Saves space and adds some clarity.
        const auto castBinding = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<BindingNode>(node);
        };
        const auto castBindings = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<BindingsNode>(node);
        };
        const auto castCmd = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<CmdNode>(node);
        };
        const auto castExpr = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<ExprNode>(node);
        };
        const auto castExprs = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<ExprsNode>(node);
        };
        const auto castStmt = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StmtNode>(node);
        };
        const auto castStmts = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StmtsNode>(node);
        };
        const auto castString = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StringNode>(node);
        };
        const auto castType = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<TypeNode>(node);
        };
        const auto castVariable = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<VariableNode>(node);
        };

        return {{//  PRGM -> STMT
                 {{ASTNodeType::PRGM, {ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<PrgmNode>(std::vector<StmtNodePtr> {castStmt(productionList[0].node)});
                  }},

                 //  STMT -> EXPR "[SEMICOLON]"
                 {{ASTNodeType::STMT, {ASTNodeType::EXPR, TokenType::SEMICOLON}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ExprStmtNode>(castExpr(productionList[0].node));
                  }},

                 //  EXPR -> "[INTVAL]"
                 {{ASTNodeType::EXPR, {TokenType::INTVAL}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<IntExprNode>(std::stoll(productionList[0].token->contents));
                  }},
                 //  EXPR -> "[FLOATVAL]"
                 {{ASTNodeType::EXPR, {TokenType::FLOATVAL}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<FloatExprNode>(std::stold(productionList[0].token->contents));
                  }},
                 //  EXPR -> "[TRUE]"
                 {{ASTNodeType::EXPR, {TokenType::TRUE}},
                  [&](const ProductionItemList&) { return std::make_shared<BoolExprNode>(true); }},
                 //  EXPR -> "[FALSE]"
                 {{ASTNodeType::EXPR, {TokenType::FALSE}},
                  [&](const ProductionItemList&) { return std::make_shared<BoolExprNode>(false); }},
                 //  EXPR -> EXPR "[QUESTION]" EXPR "[COLON]" EXPR
                 {{ASTNodeType::EXPR,
                   {ASTNodeType::EXPR, TokenType::QUESTION, ASTNodeType::EXPR, TokenType::COLON, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<TernaryExprNode>(castExpr(productionList[0].node),
                                                               castExpr(productionList[2].node),
                                                               castExpr(productionList[4].node));
                  }},
                 //  EXPR -> EXPR "[EXPONENTIATE]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::EXPONENTIATE, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::EXPONENTIATE,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MULTIPLY]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MULTIPLY, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::MULTIPLY,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[DIVIDE]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DIVIDE, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::DIVIDE,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MODULO]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MODULO, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::MODULO,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[ADD]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::ADD, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::ADD,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MINUS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MINUS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::SUBTRACT,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[LESSER]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::LESSER,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[LESSER_EQUAL]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER_EQUAL, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::LESSER_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[GREATER]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::GREATER,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[GREATER_EQUAL]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER_EQUAL, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::GREATER_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[DOUBLE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DOUBLE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[NOT_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::NOT_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::NOT_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[AND]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::AND, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::AND,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[OR]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::OR, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::OR,
                                                             castExpr(productionList[2].node));
                  }},

                 //  EXPR -> VARIABLE "[EXPONENTIATE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EXPONENTIATE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::EXPONENTIATE_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MULTIPLY_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MULTIPLY_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::MULTIPLY_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[DIVIDE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::DIVIDE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::DIVIDE_EQUAL, castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MODULO_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MODULO_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::MODULO_EQUAL, castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[ADD_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::ADD_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node), BinOpType::ADD_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MINUS_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MINUS_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::SUBTRACT_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node), BinOpType::EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<VarExprNode>(castVariable(productionList[0].node));
                  }},
                 //  EXPR -> "[(]" EXPR "[)]"
                 {{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN}},
                  [&](const ProductionItemList& productionList) { return productionList[1].node; }},

                 //  VARIABLE -> "[VARIABLE]"
                 {{ASTNodeType::VARIABLE, {TokenType::VARIABLE}}, [&](const ProductionItemList& productionList) {
                      return std::make_shared<IdentVariableNode>(productionList[0].token->contents);
                  }}}};
    }

    std::vector<std::tuple<Grammar::GrammarRule, Grammar::NodeConstructor>> Grammar::_defineGrammar() {
        //  Some shorthands for `std::reinterpret_pointer_cast`.
        //  Saves space and adds some clarity.
        const auto castBinding = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<BindingNode>(node);
        };
        const auto castBindings = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<BindingsNode>(node);
        };
        const auto castExpr = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<ExprNode>(node);
        };
        const auto castExprs = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<ExprsNode>(node);
        };
        const auto castStmt = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StmtNode>(node);
        };
        const auto castStmts = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StmtsNode>(node);
        };
        const auto castString = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<StringNode>(node);
        };
        const auto castType = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<TypeNode>(node);
        };
        const auto castVariable = [](const std::shared_ptr<ASTNode>& node) {
            return std::reinterpret_pointer_cast<VariableNode>(node);
        };

        return {{//  Meta:
                 //  -----

                 //  PRGM -> STMTS
                 {{ASTNodeType::PRGM, {ASTNodeType::STMTS}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<PrgmNode>(castStmts(productionList[0].node)->stmts);
                  }},

                 //  BINDINGS -> BINDING
                 {{ASTNodeType::BINDINGS, {ASTNodeType::BINDING}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BindingsNode>(castBinding(productionList[0].node));
                  }},
                 //  BINDINGS -> BINDINGS "[COMMA]" BINDING
                 {{ASTNodeType::BINDINGS, {ASTNodeType::BINDINGS, TokenType::COMMA, ASTNodeType::BINDING}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BindingsNode>(castBindings(productionList[0].node)->bindings,
                                                            castBinding(productionList[2].node));
                  }},

                 //  EXPRS -> EXPR
                 {{ASTNodeType::EXPRS, {ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ExprsNode>(castExpr(productionList[0].node));
                  }},
                 //  EXPRS -> EXPRS "[COMMA]" EXPR
                 {{ASTNodeType::EXPRS, {ASTNodeType::EXPRS, TokenType::COMMA, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ExprsNode>(castExprs(productionList[0].node)->exprs,
                                                         castExpr(productionList[0].node));
                  }},

                 //  STMTS -> STMT
                 {{ASTNodeType::STMTS, {ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<StmtsNode>(castStmt(productionList[0].node));
                  }},
                 //  STMTS -> STMTS STMT
                 {{ASTNodeType::STMTS, {ASTNodeType::STMTS, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<StmtsNode>(castStmts(productionList[0].node)->stmts,
                                                         castStmt(productionList[1].node));
                  }},

                 //  STRING -> "[STRINGVAL]"
                 {{ASTNodeType::STRING, {TokenType::STRINGVAL}},
                  [&](const ProductionItemList&) { return std::make_shared<StringNode>(); }},


                 //  Bindings:
                 //  ---------

                 //  BINDING -> "[VARIABLE]"
                 {{ASTNodeType::BINDING, {TokenType::VARIABLE}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BindingNode>(productionList[0].token->contents);
                  }},
                 //  BINDING -> "[VARIABLE]" "[COLON]" TYPE
                 {{ASTNodeType::BINDING, {TokenType::VARIABLE, TokenType::COLON, ASTNodeType::TYPE}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BindingNode>(productionList[0].token->contents,
                                                           castType(productionList[2].node));
                  }},


                 //  Expressions:
                 //  ------------

                 //  EXPR -> "[INTVAL]"
                 {{ASTNodeType::EXPR, {TokenType::INTVAL}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<IntExprNode>(std::stoll(productionList[0].token->contents));
                  }},
                 //  EXPR -> "[FLOATVAL]"
                 {{ASTNodeType::EXPR, {TokenType::FLOATVAL}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<FloatExprNode>(std::stold(productionList[0].token->contents));
                  }},
                 //  EXPR -> STRING
                 {{ASTNodeType::EXPR, {ASTNodeType::STRING}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<StringExprNode>(castString(productionList[0].node));
                  }},
                 //  EXPR -> "[TRUE]"
                 {{ASTNodeType::EXPR, {TokenType::TRUE}},
                  [&](const ProductionItemList&) { return std::make_shared<BoolExprNode>(true); }},
                 //  EXPR -> "[FALSE]"
                 {{ASTNodeType::EXPR, {TokenType::FALSE}},
                  [&](const ProductionItemList&) { return std::make_shared<BoolExprNode>(false); }},
                 //  EXPR -> "[LSQUARE]" EXPRS "[RSQUARE]"
                 {{ASTNodeType::EXPR, {TokenType::LSQUARE, ASTNodeType::EXPRS, TokenType::RSQUARE}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ArrayExprNode>(castExprs(productionList[0].node)->exprs);
                  }},
                 //  EXPR -> "[LPAREN]" EXPRS "[RPAREN]"
                 {{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::EXPRS, TokenType::RPAREN}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<TupleExprNode>(castExprs(productionList[0].node)->exprs);
                  }},
                 //  EXPR -> EXPR "[QUESTION]" EXPR "[COLON]" EXPR
                 {{ASTNodeType::EXPR,
                   {ASTNodeType::EXPR, TokenType::QUESTION, ASTNodeType::EXPR, TokenType::COLON, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<TernaryExprNode>(castExpr(productionList[0].node),
                                                               castExpr(productionList[2].node),
                                                               castExpr(productionList[4].node));
                  }},
                 //  //  EXPR -> "[BACKTICK]" CMD "[BACKTICK]"
                 //  {{ASTNodeType::EXPR, {TokenType::BACKTICK, ASTNodeType::CMD, TokenType::BACKTICK}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdExprNode>(castCmd(productionList[1].node));
                 //   }},
                 //  EXPR -> "[LPAREN]" BINDINGS "[RPAREN]" "[COLON]" TYPE STMT
                 {{ASTNodeType::EXPR,
                   {TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::COLON, ASTNodeType::TYPE,
                    ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<LambdaExprNode>(castBindings(productionList[1].node)->bindings,
                                                              castStmt(productionList[5].node),
                                                              castType(productionList[4].node));
                  }},
                 //  EXPR -> "[LPAREN]" BINDINGS "[RPAREN]" STMT
                 {{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<LambdaExprNode>(castBindings(productionList[1].node)->bindings,
                                                              castStmt(productionList[3].node));
                  }},

                 //  EXPR -> "[LPAREN]" BINDINGS "[RPAREN]" "[COLON]" TYPE EXPR
                 {{ASTNodeType::EXPR,
                   {TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, TokenType::COLON, ASTNodeType::TYPE,
                    ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<LambdaExprNode>(castBindings(productionList[1].node)->bindings,
                                                              castExpr(productionList[5].node),
                                                              castType(productionList[4].node));
                  }},
                 //  EXPR -> "[LPAREN]" BINDINGS "[RPAREN]" EXPR
                 {{ASTNodeType::EXPR, {TokenType::LPAREN, ASTNodeType::BINDINGS, TokenType::RPAREN, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<LambdaExprNode>(castBindings(productionList[1].node)->bindings,
                                                              castExpr(productionList[3].node));
                  }},

                 //  EXPR -> EXPR "[INCREMENT]"
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::INCREMENT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<UnOpExprNode>(UnOpType::POST_INCREMENT,
                                                            castVariable(productionList[0].node));
                  }},
                 //  EXPR -> EXPR "[DECREMENT]"
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DECREMENT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<UnOpExprNode>(UnOpType::POST_DECREMENT, castExpr(productionList[0].node));
                  }},
                 //  EXPR -> "[NOT]" EXPR
                 {{ASTNodeType::EXPR, {TokenType::NOT, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<UnOpExprNode>(UnOpType::NOT, castExpr(productionList[0].node));
                  }},
                 //  EXPR -> "[INCREMENT]" EXPR
                 {{ASTNodeType::EXPR, {TokenType::INCREMENT, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<UnOpExprNode>(UnOpType::PRE_INCREMENT, castExpr(productionList[0].node));
                  }},
                 //  EXPR -> "[DECREMENT]" EXPR
                 {{ASTNodeType::EXPR, {TokenType::DECREMENT, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<UnOpExprNode>(UnOpType::PRE_DECREMENT, castExpr(productionList[0].node));
                  }},
                 //  EXPR -> EXPR "[EXPONENTIATE]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::EXPONENTIATE, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::EXPONENTIATE,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MULTIPLY]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MULTIPLY, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::MULTIPLY,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[DIVIDE]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DIVIDE, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::DIVIDE,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MODULO]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MODULO, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::MODULO,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[ADD]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::ADD, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::ADD,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[MINUS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::MINUS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::SUBTRACT,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[LESSER]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::LESSER,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[LESSER_EQUAL]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::LESSER_EQUAL, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::LESSER_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[GREATER]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::GREATER,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[GREATER_EQUAL]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::GREATER_EQUAL, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::GREATER_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[DOUBLE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::DOUBLE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[NOT_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::NOT_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::NOT_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[AND]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::AND, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::AND,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> EXPR "[OR]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::EXPR, TokenType::OR, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castExpr(productionList[0].node), BinOpType::OR,
                                                             castExpr(productionList[2].node));
                  }},

                 //  EXPR -> VARIABLE "[EXPONENTIATE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EXPONENTIATE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::EXPONENTIATE_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MULTIPLY_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MULTIPLY_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::MULTIPLY_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[DIVIDE_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::DIVIDE_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::DIVIDE_EQUAL, castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MODULO_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MODULO_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::MODULO_EQUAL, castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[ADD_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::ADD_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node), BinOpType::ADD_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[MINUS_EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::MINUS_EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node),
                                                             BinOpType::SUBTRACT_EQUAL,
                                                             castExpr(productionList[2].node));
                  }},
                 //  EXPR -> VARIABLE "[EQUALS]" EXPR
                 {{ASTNodeType::EXPR, {ASTNodeType::VARIABLE, TokenType::EQUALS, ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<BinOpExprNode>(castVariable(productionList[0].node), BinOpType::EQUAL,
                                                             castExpr(productionList[2].node));
                  }},


                 //  Commands:
                 //  ---------

                 //  //  CMD -> "[CMDSTRINGVAL]"
                 //  {{ASTNodeType::CMD, {TokenType::CMDSTRINGVAL}},
                 //   [&](const ProductionItemList& productionList) { return std::make_shared<CmdNode>(); }},
                 //  //  CMD -> STRING
                 //  {{ASTNodeType::CMD, {ASTNodeType::STRING}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castString(productionList[].node), );
                 //   }},
                 //  //  CMD -> VARIABLE
                 //  {{ASTNodeType::CMD, {ASTNodeType::VARIABLE}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castVariable(productionList[].node), );
                 //   }},
                 //  //  CMD -> CMD "[CMDSTRINGVAL]"
                 //  {{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::CMDSTRINGVAL}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castCmd(productionList[].node), );
                 //   }},
                 //  //  CMD -> CMD STRING
                 //  {{ASTNodeType::CMD, {ASTNodeType::CMD, ASTNodeType::STRING}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castCmd(productionList[].node),
                 //                                        castString(productionList[].node), );
                 //   }},
                 //  //  CMD -> CMD VARIABLE
                 //  {{ASTNodeType::CMD, {ASTNodeType::CMD, ASTNodeType::VARIABLE}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castCmd(productionList[].node),
                 //                                        castVariable(productionList[].node), );
                 //   }},
                 //  //  CMD -> CMD "[AMPERSAND]"
                 //  {{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::AMPERSAND}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castCmd(productionList[].node), );
                 //   }},
                 //  //  CMD -> CMD "[PIPE]" CMD
                 //  {{ASTNodeType::CMD, {ASTNodeType::CMD, TokenType::PIPE, ASTNodeType::CMD}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdNode>(castCmd(productionList[].node),
                 //       castCmd(productionList[].node), );
                 //   }},


                 //  Statements:
                 //  -----------

                 //  STMT -> "[ALIAS]" "[VARIABLE]" "[EQUALS]" CMD "[SEMICOLON]"
                 //  {{ASTNodeType::STMT, {TokenType::ALIAS, TokenType::VARIABLE, TokenType::EQUALS, ASTNodeType::CMD,
                 //  TokenType::SEMICOLON}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<AliasStmtNode>(productionList[1].token->contents,
                 //                                              castCmd(productionList[3].node));
                 //   }},
                 //  STMT -> "[IF]" "[LPAREN]" EXPR "[RPAREN]" STMT "[ELSE]" STMT
                 {{ASTNodeType::STMT,
                   {TokenType::IF, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT,
                    TokenType::ELSE, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<IfStmtNode>(std::vector<ExprNodePtr> {castExpr(productionList[2].node)},
                                                          std::vector<StmtNodePtr> {castStmt(productionList[4].node)},
                                                          castStmt(productionList[6].node));
                  }},
                 //  STMT -> "[IF]" "[LPAREN]" EXPR "[RPAREN]" STMT
                 {{ASTNodeType::STMT,
                   {TokenType::IF, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<IfStmtNode>(std::vector<ExprNodePtr> {castExpr(productionList[2].node)},
                                                          std::vector<StmtNodePtr> {castStmt(productionList[4].node)});
                  }},
                 //  STMT -> "[FOR]" "[LPAREN]" STMT "[SEMICOLON]" EXPR "[SEMICOLON]" STMT "[RPAREN]" STMT
                 {{ASTNodeType::STMT,
                   {TokenType::FOR, TokenType::LPAREN, ASTNodeType::STMT, TokenType::SEMICOLON, ASTNodeType::EXPR,
                    TokenType::SEMICOLON, ASTNodeType::STMT, TokenType::RPAREN, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ForStmtNode>(
                              castStmt(productionList[2].node), castExpr(productionList[4].node),
                              castStmt(productionList[6].node), castStmt(productionList[8].node));
                  }},
                 //  STMT -> "[WHILE]" "[LPAREN]" EXPR "[RPAREN]" STMT
                 {{ASTNodeType::STMT,
                   {TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR, TokenType::RPAREN, ASTNodeType::STMT}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<WhileStmtNode>(castExpr(productionList[2].node),
                                                             castStmt(productionList[4].node));
                  }},
                 //  STMT -> "[DO]" STMT "[WHILE]" "[LPAREN]" EXPR "[RPAREN]"
                 {{ASTNodeType::STMT,
                   {TokenType::DO, ASTNodeType::STMT, TokenType::WHILE, TokenType::LPAREN, ASTNodeType::EXPR,
                    TokenType::RPAREN}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<DoWhileStmtNode>(castExpr(productionList[4].node),
                                                               castStmt(productionList[1].node));
                  }},
                 //  STMT -> "[LCURLY]" STMTS "[RCURLY]"
                 {{ASTNodeType::STMT, {TokenType::LCURLY, ASTNodeType::STMTS, TokenType::RCURLY}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ScopeStmtNode>(castStmts(productionList[1].node)->stmts);
                  }},
                 //  //  STMT -> CMD "[SEMICOLON]"
                 //  {{ASTNodeType::STMT, {ASTNodeType::CMD, TokenType::SEMICOLON}},
                 //   [&](const ProductionItemList& productionList) {
                 //       return std::make_shared<CmdStmtNode>(castCmd(productionList[0].node));
                 //   }},
                 //  STMT -> EXPR
                 {{ASTNodeType::STMT, {ASTNodeType::EXPR}},
                  [&](const ProductionItemList& productionList) {
                      return std::make_shared<ExprStmtNode>(castExpr(productionList[0].node));
                  }},


                 //  Types:
                 //  ------
                 //  TYPE -> "[INT]"
                 {{ASTNodeType::TYPE, {TokenType::INT}},
                  [&](const ProductionItemList&) { return std::make_shared<IntTypeNode>(); }},
                 //  TYPE -> "[FLOAT]"
                 {{ASTNodeType::TYPE, {TokenType::FLOAT}},
                  [&](const ProductionItemList&) { return std::make_shared<FloatTypeNode>(); }},


                 //  Variables:
                 //  ----------

                 //  VARIABLE -> "[VARIABLE]"
                 {{ASTNodeType::VARIABLE, {TokenType::VARIABLE}}, [&](const ProductionItemList& productionList) {
                      return std::make_shared<IdentVariableNode>(productionList[0].token->contents);
                  }}}};
    }
#pragma clang diagnostic pop

}  //  namespace Parser
