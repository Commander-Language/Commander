#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "../parser/parser.hpp"

namespace FlowController {
    class FlowController {
    public:
        FlowController(Parser::ASTNodeList&);

    private:
        Parser::ASTNodeList _nodes;
    };
}  // namespace FlowController
#endif
