#include "Statement.h"
#include "AstVisitor.h"
#include "IR/IRGeneratorLLVM.h"

void VariableDeclaration::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* VariableDeclaration::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void VariableAssignment::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* VariableAssignment::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void ReturnStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* ReturnStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void Block::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* Block::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void WhileLoop::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* WhileLoop::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void ForLoop::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* ForLoop::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void IfStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* IfStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void FunctionDeclaration::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* FunctionDeclaration::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void ExpressionStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* ExpressionStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void Program::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void Program::addDeclaration(std::unique_ptr<Statement> statement) {
    if (statement) {
        // Safety check to ensure we don't add nulls
        statements.push_back(std::move(statement));
    }
}

llvm::Value* Program::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}
