#ifndef COMPILER_PROJECT_PRINTERVISITOR_H
#define COMPILER_PROJECT_PRINTERVISITOR_H

#include "Visitor.h"

class ASTPrinter : public Visitor {
public:
    ~ASTPrinter() override = default;

    void visit(Program &program) override;

private:
    void visit(IntegerLiteralNode &node) override;

    void visit(StringLiteralNode &node) override;

    void visit(BooleanLiteralNode &node) override;

    void visit(BinaryExpression &node) override;

    void visit(UnaryExpression &node) override;

    void visit(IdentifierNode &node) override;

    void visit(CallExpression &node) override;

    void visit(VariableDeclaration &node) override;

    void visit(VariableAssignment &node) override;

    void visit(ReturnStatement &node) override;

    void visit(Block &node) override;

    void visit(WhileStatement &node) override;

    void visit(IfStatement &node) override;

    void visit(FunctionDeclaration &node) override;

    void visit(FunctionCall &node) override;
};


#endif //COMPILER_PROJECT_PRINTERVISITOR_H
