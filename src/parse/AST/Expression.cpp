#include "Expression.h"

void IntegerLiteralNode::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void IdentifierNode::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void CallExpression::accept(const Visitor &visitor) {
    visitor.visit(*this);
}
