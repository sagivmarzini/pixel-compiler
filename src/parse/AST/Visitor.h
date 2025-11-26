#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

struct IntegerLiteralNode;
struct StringLiteralNode;
struct BooleanLiteralNode;
struct BinaryExpression;
struct UnaryExpression;
struct IdentifierNode;
struct CallExpression;

struct VariableDeclaration;
struct VariableAssignment;
struct ReturnStatement;
struct Block;
struct WhileStatement;
struct IfStatement;
struct FunctionDeclaration;
struct FunctionCall;

struct Program;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(const IntegerLiteralNode &node) const = 0;

    virtual void visit(const StringLiteralNode &node) const = 0;

    virtual void visit(const BooleanLiteralNode &node) const = 0;

    virtual void visit(const BinaryExpression &node) const = 0;

    virtual void visit(const UnaryExpression &node) const = 0;

    virtual void visit(const IdentifierNode &node) const = 0;

    virtual void visit(const CallExpression &node) const = 0;

    virtual void visit(const VariableDeclaration &node) const = 0;

    virtual void visit(const VariableAssignment &node) const = 0;

    virtual void visit(const ReturnStatement &node) const = 0;

    virtual void visit(const Block &node) const = 0;

    virtual void visit(const WhileStatement &node) const = 0;

    virtual void visit(const IfStatement &node) const = 0;

    virtual void visit(const FunctionDeclaration &node) const = 0;

    virtual void visit(const FunctionCall &node) const = 0;

    virtual void visit(const Program &program) const = 0;
};

#endif //COMPILER_PROJECT_VISITOR_H
