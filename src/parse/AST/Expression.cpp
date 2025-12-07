#include "Expression.h"
#include "Visitor.h"

void IntegerLiteralNode::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void IdentifierNode::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(const Visitor& visitor) {
    visitor.visit(*this);
}

void RangeExpression::accept(const Visitor& visitor) {
    visitor.visit(*this);
}
