#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

namespace AST {
    struct FunctionDeclaration;
    struct Program;
    struct ForLoop;
    struct ExpressionStatement;
    struct FloatLiteralNode;
    struct RangeExpression;
    struct IntegerLiteralNode;
    struct StringLiteralNode;
    struct BooleanLiteralNode;
    struct BinaryExpression;
    struct UnaryExpression;
    struct VariableExpression;
    struct IncDecExpression;
    struct FunctionCall;
    struct VariableDeclaration;
    struct VariableAssignment;
    struct ReturnStatement;
    struct Block;
    struct WhileLoop;
    struct IfStatement;
}


class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void visit(AST::Program& program) = 0;

    virtual void visit(AST::FunctionDeclaration& node) = 0;

    virtual void visit(AST::ExpressionStatement& node) = 0;

    virtual void visit(AST::IfStatement& node) = 0;

    virtual void visit(AST::WhileLoop& node) = 0;

    virtual void visit(AST::ForLoop& node) = 0;

    virtual void visit(AST::Block& node) = 0;

    virtual void visit(AST::VariableDeclaration& node) = 0;

    virtual void visit(AST::FunctionCall& node) = 0;

    virtual void visit(AST::BinaryExpression& node) = 0;

    virtual void visit(AST::UnaryExpression& node) = 0;

    virtual void visit(AST::IncDecExpression& node) = 0;

    virtual void visit(AST::VariableAssignment& node) = 0;

    virtual void visit(AST::ReturnStatement& node) = 0;

    virtual void visit(AST::IntegerLiteralNode& node) = 0;

    virtual void visit(AST::StringLiteralNode& node) = 0;

    virtual void visit(AST::BooleanLiteralNode& node) = 0;

    virtual void visit(AST::VariableExpression& node) = 0;

    virtual void visit(AST::RangeExpression& node) = 0;

    virtual void visit(AST::FloatLiteralNode& node) = 0;
};

#endif //COMPILER_PROJECT_VISITOR_H
