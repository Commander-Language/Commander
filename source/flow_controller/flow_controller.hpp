/**
 * @file flow_controller.hpp
 * @brief Definition for flow controller components
 */

#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "source/builtin_functions/functions.hpp"
#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"
#include "source/util/print.hpp"
#include "source/type_checker/type.hpp"

#include <any>
#include <cmath>
#include <iostream>
#include <ncurses.h>
#include <tuple>

namespace FlowController {

    /**
     * @brief A Commander array representation
     */
    template<typename TType>
    struct CommanderArray {
        /**
         * @brief The values in the array
         */
        std::vector<TType> values;
        /**
         * @brief The type of the values in the array
         */
        TypeChecker::Type type;
    };
    /**
     * @brief A Commander bool representation
     */
    using CommanderBool = bool;
    /**
     * @brief A Commander tuple representation
     */
    struct CommanderTuple {
        /**
         * @brief The values of a tuple
         */
        std::vector<std::any> values;
        /**
         * @brief The types of the values in the tuple
         * @details This should be a 1-to-1 mapping with the values list.
         */
        std::vector<TypeChecker::Type> types;
    };
    /**
     * @brief A Commander string representation
     */
    using CommanderString = std::string;
    /**
     * @brief A Commander int representation
     */
    using CommanderInt = int64_t;
    /**
     * @brief A Commander float representation
     */
    using CommanderFloat = double;

    /**
     * @brief A commander lambda representation
     */
    struct CommanderLambda {
        CommanderLambda(Parser::BindingsNodePtr, Parser::StmtNodePtr);
        Parser::BindingsNodePtr bindings;
        Parser::StmtNodePtr body;
        TypeChecker::Type returnType;
    };


    /**
     * @brief Interpret the AST Nodes given from the Parser
     * @details
     *    The responsibilities of the Flow Controller will include interpreting the AST nodes,
     *    keeping track of variables/functions in a symbol table, and calling terminal programs.
     */
    class FlowController {
    public:
        /**
         * @brief Class Constructor
         */
        FlowController(Parser::ASTNodeList &);

        /**
         * @brief Parse the AST Nodes
         */
        void runtime();

        /**
         * @brief Check if flow controller could find a varible
         * @returns True or False
         */
        bool hasVariable(const std::string&);

        /**
         * @brief Get value of variable
         */
        TypeChecker::CommanderInt getVariableValue(const std::string&);

    private:
        /**
         * @brief The AST nodes in a list
         */
        Parser::ASTNodeList _nodes;

        /**
         * @brief The symbol table
         */
        SymbolTableOrganizer _symbolTable;

        /**
         * @brief Set a variable in the symbol table.
         * @param name The name of the symbol
         * @param value The value of the symbol
         */
        void _setVariable(const std::string& name, std::any value);

        /**
         * @brief Get a variable from the symbol table.
         * @param name The name of the symbol
         * @returns The value of the symbol
         */
        std::any _getVariable(const std::string &name);

        /**
         * @brief Run a command
         */
        void _runCommand();

        /**
         * @brief Get a String representation of a Commander type value
         */
        static std::string _commanderTypeToString(std::any value, TypeChecker::Type type,
                                                  TypeChecker::Type subtype = TypeChecker::Type::INT);


        /**
         * @brief Helper to interpret binding nodes
         */
        void _binding(const Parser::BindingNodePtr &);

        /**
         * @brief Helper to interpret bindings nodes
         */
        void _bindings(const Parser::BindingsNodePtr &);

        /**
         * @brief Helper to interpret cmd nodes
         */
        void _cmd(const Parser::CmdNodePtr &);

        /**
         * @brief Helper to interpret expr nodes
         */
        std::any _expr(const Parser::ExprNodePtr &);

        /**
         * @brief Helper to interpret exprs nodes
         */
        void _exprs(const Parser::ExprsNodePtr &);

        /**
         * @brief Helper to interpret prgm nodes
         */
        void _prgm(const std::shared_ptr<Parser::PrgmNode> &);

        /**
         * @brief Helper to interpret stmt nodes
         * @details Some statements need to return some generic value;
         * for example the body of a function call, so return value or nullptr if unnecessary.
         * @returns Generic value or nullptr if not needed.
         */
        std::any _stmt(const Parser::StmtNodePtr &);

        /**
         * @brief Helper to interpret stmts nodes
         */
        void _stmts(const Parser::StmtsNodePtr &);

        /**
         * @brief Helper to interpret string nodes
         */
        std::string _string(const Parser::StringNodePtr &);

        /**
         * @brief Helper to interpret types nodes
         * @param types The types to interpret
         */
        void _types(const Parser::TypesNodePtr& types);
        /**
         * @brief Helper to interpret type nodes
         */
        void _type(const Parser::TypeNodePtr &);

        /**
         * @brief Helper to interpret variable nodes
         */
        void _variable(const Parser::VariableNodePtr &);

        /**
         * @brief Helper to interpret unary operations
         */
        std::any _unaryOp(std::shared_ptr<Parser::UnOpExprNode> &);

        /**
         * @brief Helper to interpret binary operations
         */
        std::any _binaryOp(std::shared_ptr<Parser::BinOpExprNode> &binOp);

        /**
         *
         * @return
         */
        template<typename TType>
        static std::string _helper(CommanderArray<TType> arr) {
            std::string output = "[";
            for (int i = 0; i < arr.values.size(); i++) {
                if (i < arr.values.size() - 1)
                    output.append(output + _commanderTypeToString(arr.values[i], arr.type) + ", ");
                else
                    output.append(output + _commanderTypeToString(arr.values[i], arr.type) + "]");
            }
            return output;
        }
    };

}  // namespace FlowController
#endif
