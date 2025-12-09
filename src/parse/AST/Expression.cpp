#include "Expression.h"

#include "AstVisitor.h"

void IntegerLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void FloatLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void IdentifierNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void RangeExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
