#ifndef COMPILER_PROJECT_PRINTERVISITOR_H
#define COMPILER_PROJECT_PRINTERVISITOR_H

#include "Visitor.h"
#include <string>

#include "Statement.h"

class ASTPrinter : public Visitor {
public:
    ~ASTPrinter() override = default;

    void visit(const Program &node) const override;

private:
    void visit(const IntegerLiteralNode &node) const override;

    void visit(const StringLiteralNode &node) const override;

    void visit(const BooleanLiteralNode &node) const override;

    void visit(const BinaryExpression &node) const override;

    void visit(const UnaryExpression &node) const override;

    void visit(const IdentifierNode &node) const override;

    void visit(const CallExpression &node) const override;

    void visit(const VariableDeclaration &node) const override;

    void visit(const VariableAssignment &node) const override;

    void visit(const ReturnStatement &node) const override;

    void visit(const Block &node) const override;

    void visit(const WhileStatement &node) const override;

    void visit(const IfStatement &node) const override;

    void visit(const FunctionDeclaration &node) const override;

    void visit(const FunctionCall &node) const override;
};


#endif //COMPILER_PROJECT_PRINTERVISITOR_H
