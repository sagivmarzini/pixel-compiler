#include "ExpressionNodes.h"
#include "Visitor.h"

void NumberLiteral::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void BoolLiteral::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void StringLiteral::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void Variable::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void BinaryExpression::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void UnaryExpression::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

void FunctionCall::accept(Visitor &visitor)
{
    visitor.visit(*this);
}
