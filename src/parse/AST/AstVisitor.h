#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

struct ForLoop;
struct ExpressionStatement;
struct FloatLiteralNode;
struct RangeExpression;
struct IntegerLiteralNode;
struct StringLiteralNode;
struct BooleanLiteralNode;
struct BinaryExpression;
struct UnaryExpression;
struct IncDecExpression;
struct IdentifierNode;
struct FunctionCall;
struct VariableDeclaration;
struct VariableAssignment;
struct ReturnStatement;
struct Block;
struct WhileLoop;
struct IfStatement;
struct FunctionDeclaration;

struct Program;

// TODO: Templated visitor with return type - AstVisitor<void> - template<typename R> class AstVisitor
class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void visit(Program& program) = 0;

    virtual void visit(FunctionDeclaration& node) = 0;

    virtual void visit(ExpressionStatement& node) = 0;

    virtual void visit(IfStatement& node) = 0;

    virtual void visit(WhileLoop& node) = 0;

    virtual void visit(ForLoop& node) = 0;

    virtual void visit(Block& node) = 0;

    virtual void visit(VariableDeclaration& node) = 0;

    virtual void visit(FunctionCall& node) = 0;

    virtual void visit(BinaryExpression& node) = 0;

    virtual void visit(UnaryExpression& node) = 0;

    virtual void visit(IncDecExpression& node) = 0;

    virtual void visit(VariableAssignment& node) = 0;

    virtual void visit(ReturnStatement& node) = 0;

    virtual void visit(IntegerLiteralNode& node) = 0;

    virtual void visit(StringLiteralNode& node) = 0;

    virtual void visit(BooleanLiteralNode& node) = 0;

    virtual void visit(IdentifierNode& node) = 0;

    virtual void visit(RangeExpression& node) = 0;

    virtual void visit(FloatLiteralNode& node) = 0;
};

#endif //COMPILER_PROJECT_VISITOR_H
