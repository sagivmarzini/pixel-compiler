#include "Expression.h"

#include "AstVisitor.h"
#include "IR/IRGeneratorLLVM.h"

void AST::IntegerLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::IntegerLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::FloatLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::FloatLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::StringLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::StringLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::BooleanLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::BooleanLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::BinaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::BinaryExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::UnaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::UnaryExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::VariableExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::VariableExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::IncDecExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::IncDecExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::ArrayLiteral::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::ArrayLiteral::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::FunctionCall::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::FunctionCall::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void AST::RangeExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* AST::RangeExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}
