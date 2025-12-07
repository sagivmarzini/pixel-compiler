#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "Visitor.h"
#include "lex/Token.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void accept(const Visitor &visitor) = 0; // For visitor pattern

protected:
    TokenMetadata _metadata;
};

#endif //COMPILER_PROJECT_ASTNODE_H
