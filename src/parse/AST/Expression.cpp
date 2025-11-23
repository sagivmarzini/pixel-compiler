#include "Expression.h"

void IntegerLiteralNode::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void IdentifierNode::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void CallExpression::accept(Visitor &visitor) {
    visitor.visit(*this);
}
