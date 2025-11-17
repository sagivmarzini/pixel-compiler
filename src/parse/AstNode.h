#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

class Visitor; //forward declaration to solve circular include

struct AstNode
{
    virtual ~AstNode() = 0;

    virtual void access(Visitor&) = 0;
};

struct ExpressionNode : AstNode {};

struct StatementNode : AstNode {};




#endif //COMPILER_PROJECT_ASTNODE_H