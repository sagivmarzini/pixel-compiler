#ifndef COMPILER_PROJECT_STATEMENTNODE_H
#define COMPILER_PROJECT_STATEMENTNODE_H

#include "AstNode.h"

#include <vector>
#include <memory>


struct BlockNode // stuff containing { } (function, if)
{
    BlockNode()
    {
        body = std::vector<std::unique_ptr<AstNode>>();
    }

    std::vector<std::unique_ptr<AstNode>> body;

    void addNode(std::unique_ptr<AstNode> node)
    {
        body.push_back(std::move(node));
    }
};



#endif //COMPILER_PROJECT_STATEMENTNODE_H