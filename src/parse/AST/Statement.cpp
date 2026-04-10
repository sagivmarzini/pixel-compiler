#include "Statement.h"
#include "AstVisitor.h"
#include "IR/IRGeneratorLLVM.h"

void AST::VariableDeclaration::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::VariableDeclaration::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::VariableAssignment::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::VariableAssignment::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::ArrayAssignment::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::ArrayAssignment::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::ReturnStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::ReturnStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::Block::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::Block::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::WhileLoop::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::WhileLoop::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::ForLoop::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::ForLoop::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::IfStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::IfStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::FunctionDeclaration::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::FunctionDeclaration::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::ExpressionStatement::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::ExpressionStatement::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::Program::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void AST::Program::addDeclaration(std::unique_ptr<Statement> statement) {
    if (statement) {
        // Safety check to ensure we don't add nulls
        statements.push_back(std::move(statement));
    }
}

llvm::Value* AST::Program::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}
