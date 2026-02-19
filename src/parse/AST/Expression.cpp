#include "Expression.h"

#include "AstVisitor.h"
#include "IR/IRGeneratorLLVM.h"

void IntegerLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* IntegerLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void FloatLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* FloatLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void StringLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* StringLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void BooleanLiteralNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* BooleanLiteralNode::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void BinaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* BinaryExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void UnaryExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* UnaryExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void VariableExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* VariableExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void IncDecExpression::accept(AstVisitor &visitor) {
    visitor.visit(*this);
}

llvm::Value* IncDecExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void FunctionCall::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* FunctionCall::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}

void RangeExpression::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* RangeExpression::acceptIR(IRGeneratorLLVM& visitor) {
    return visitor.visit(*this);
}
