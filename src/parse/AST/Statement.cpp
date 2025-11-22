#include "Statement.h"

void VariableDeclaration::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void VariableAssignment::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void Block::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(Visitor &visitor) {
    visitor.visit(*this);
}

void Program::accept(Visitor &visitor) {
    visitor.visit(*this);
}

