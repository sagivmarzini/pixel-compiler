#ifndef COMPILER_PROJECT_DECLARATIONPASSVISITOR_H
#define COMPILER_PROJECT_DECLARATIONPASSVISITOR_H

#include "SemanticVisitor.h"


class DeclarationPassVisitor final : public SemanticVisitor {
public:
    using SemanticVisitor::SemanticVisitor; // inherit constructor

    void run(AST::AstNode& root) override;

private:
    void visit(AST::Program& program) override;

    void visit(AST::FunctionDeclaration& node) override;

    void visit(AST::IfStatement& node) override;

    void visit(AST::WhileLoop& node) override;

    void visit(AST::ForLoop& node) override;

    void visit(AST::Block& node) override;

    void visit(AST::VariableDeclaration& node) override;

    void visit(AST::FunctionCall& node) override;

    void visit(AST::BinaryExpression& node) override;

    void visit(AST::UnaryExpression& node) override;

    void visit(AST::IncDecExpression& node) override;

    void visit(AST::VariableAssignment& node) override;

    void visit(AST::ReturnStatement& node) override;

    void visit(AST::IntegerLiteralNode& node) override;

    void visit(AST::FloatLiteralNode& node) override;

    void visit(AST::StringLiteralNode& node) override;

    void visit(AST::BooleanLiteralNode& node) override;

    void visit(AST::VariableExpression& node) override;

    void visit(AST::RangeExpression& node) override;

    void visit(AST::ExpressionStatement& node) override;
};


#endif //COMPILER_PROJECT_DECLARATIONPASSVISITOR_H
