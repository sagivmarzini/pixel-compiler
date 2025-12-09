#ifndef COMPILER_PROJECT_PRINTERVISITOR_H
#define COMPILER_PROJECT_PRINTERVISITOR_H

#include "AstVisitor.h"
#include <string>

#include "Statement.h"
#include "semantic/Symbol.h"

class AstPrinter : public AstVisitor {
public:
    ~AstPrinter() override = default;

    void print(AstNode& root);

private:
    int _indent = 0;

    void printIndent() const;

    void printSymbol(const Symbol& symbol);

    std::string symbolKindToString(Symbol::SymbolKind kind);

    void visit(Program& program) override;

    void visit(IntegerLiteralNode& node) override;

    void visit(FloatLiteralNode& node) override;

    void visit(StringLiteralNode& node) override;

    void visit(BooleanLiteralNode& node) override;

    void visit(BinaryExpression& node) override;

    void visit(UnaryExpression& node) override;

    void visit(IdentifierNode& node) override;

    void visit(FunctionCall& node) override;

    void visit(VariableDeclaration& node) override;

    void visit(VariableAssignment& node) override;

    void visit(ReturnStatement& node) override;

    void visit(ExpressionStatement& node) override;

    void visit(Block& node) override;

    void visit(WhileLoop& node) override;

    void visit(ForLoop& node) override;

    void visit(IfStatement& node) override;

    void visit(FunctionDeclaration& node) override;

    void visit(RangeExpression& node) override;
};


#endif //COMPILER_PROJECT_PRINTERVISITOR_H
