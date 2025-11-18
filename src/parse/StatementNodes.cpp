#include "StatementNodes.h"
#include "Visitor.h"

void BlockNode::addNode(std::unique_ptr<AstNode> node)
{
    body.push_back(std::move(node));
}

void VariableDeclaration::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void Function::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void Return::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void If::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void While::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void VariableAssignment::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void FunctionCallStatement::accept(Visitor &visitor)
{
    visitor.visit(*this);
}
