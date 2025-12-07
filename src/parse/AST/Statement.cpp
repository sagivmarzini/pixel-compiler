#include "Statement.h"

void VariableDeclaration::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void VariableAssignment::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void Block::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void WhileLoop::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void ForLoop::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

void Program::accept(const Visitor &visitor) {
    visitor.visit(*this);
}

