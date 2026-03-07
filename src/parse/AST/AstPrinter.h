#ifndef COMPILER_PROJECT_PRINTERVISITOR_H
#define COMPILER_PROJECT_PRINTERVISITOR_H

#include "AstVisitor.h"
#include <string>

#include "Statement.h"
#include "semantic/Symbol.h"

namespace AST {
    class Node;
}

class AstPrinter : public AstVisitor {
public:
    ~AstPrinter() override = default;

    void print(AST::AstNode& root);

private:
    int _indent = 0;

    void printIndent() const;

    void printSymbol(const Symbol& symbol);

    std::string symbolKindToString(Symbol::SymbolKind kind);

    void visit(AST::Program& program) override;

    void visit(AST::IntegerLiteralNode& node) override;

    void visit(AST::FloatLiteralNode& node) override;

    void visit(AST::StringLiteralNode& node) override;

    void visit(AST::BooleanLiteralNode& node) override;

    void visit(AST::BinaryExpression& node) override;

    void visit(AST::UnaryExpression& node) override;

    void visit(AST::VariableExpression& node) override;

    void visit(AST::IncDecExpression& node) override;

    void visit(AST::FunctionCall& node) override;

    void visit(AST::VariableDeclaration& node) override;

    void visit(AST::VariableAssignment& node) override;

    void visit(AST::ReturnStatement& node) override;

    void visit(AST::ExpressionStatement& node) override;

    void visit(AST::Block& node) override;

    void visit(AST::WhileLoop& node) override;

    void visit(AST::ForLoop& node) override;

    void visit(AST::IfStatement& node) override;

    void visit(AST::FunctionDeclaration& node) override;

    void visit(AST::RangeExpression& node) override;
};


#endif //COMPILER_PROJECT_PRINTERVISITOR_H
