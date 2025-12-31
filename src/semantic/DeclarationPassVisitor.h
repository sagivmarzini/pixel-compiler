//
// Created by Sagiv Marzini on 05/12/2025.
//

#ifndef COMPILER_PROJECT_DECLARATIONPASSVISITOR_H
#define COMPILER_PROJECT_DECLARATIONPASSVISITOR_H
#include <string>

#include "SemanticVisitor.h"


class AstNode;

class DeclarationPassVisitor final : public SemanticVisitor {
public:
    using SemanticVisitor::SemanticVisitor; // inherit constructor

    void run(AstNode& root) override;

private:
    void visit(Program& program) override;

    void visit(FunctionDeclaration& node) override;

    void visit(IfStatement& node) override;

    void visit(WhileLoop& node) override;

    void visit(ForLoop& node) override;

    void visit(Block& node) override;

    void visit(VariableDeclaration& node) override;

    void visit(FunctionCall& node) override;

    void visit(BinaryExpression& node) override;

    void visit(UnaryExpression& node) override;

    void visit(VariableAssignment& node) override;

    void visit(ReturnStatement& node) override;

    void visit(IntegerLiteralNode& node) override;

    void visit(FloatLiteralNode& node) override;

    void visit(StringLiteralNode& node) override;

    void visit(BooleanLiteralNode& node) override;

    void visit(IdentifierNode& node) override;

    void visit(RangeExpression& node) override;

    void visit(ExpressionStatement& node) override;
};


#endif //COMPILER_PROJECT_DECLARATIONPASSVISITOR_H
