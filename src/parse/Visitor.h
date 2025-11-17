#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

//expressions
struct NumberLiteral;
struct BoolLiteral;
struct Identifier;
struct BinaryExpression;
struct UnaryExpression;
struct FunctionCall;


class Visitor
{
public:
    virtual ~Visitor() = 0;

    virtual void visit(NumberLiteral&) = 0;
    virtual void visit(BoolLiteral&) = 0;
    virtual void visit(Identifier&) = 0;
    virtual void visit(BinaryExpression&) = 0;
    virtual void visit(UnaryExpression&) = 0;
    virtual void visit(FunctionCall&) = 0;
};

#endif //COMPILER_PROJECT_VISITOR_H