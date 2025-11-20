#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "Visitor.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void accept(ASTVisitor &visitor) = 0; // For visitor pattern
    // TODO: Save original AST node location in source code
};

#endif //COMPILER_PROJECT_ASTNODE_H
