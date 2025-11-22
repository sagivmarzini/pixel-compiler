#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

struct IntegerLiteralNode;
struct StringLiteralNode;
struct BinaryExpression;
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
    virtual ~Visitor() = 0;

    virtual void visit(IntegerLiteralNode &node) = 0;

    virtual void visit(StringLiteralNode &node) = 0;

    virtual void visit(BinaryExpression &node) = 0;

    virtual void visit(IdentifierNode &node) = 0;

    virtual void visit(CallExpression &node) = 0;

    virtual void visit(VariableDeclaration &node) = 0;

    virtual void visit(VariableAssignment &node) = 0;

    virtual void visit(ReturnStatement &node) = 0;

    virtual void visit(Block &node) = 0;

    virtual void visit(WhileStatement &node) = 0;

    virtual void visit(IfStatement &node) = 0;

    virtual void visit(FunctionDeclaration &node) = 0;

    virtual void visit(FunctionCall &node) = 0;

    virtual void visit(Program &program) = 0;
};


#endif //COMPILER_PROJECT_VISITOR_H
