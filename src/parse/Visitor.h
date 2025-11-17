#ifndef COMPILER_PROJECT_VISITOR_H
#define COMPILER_PROJECT_VISITOR_H

//expressions
struct NumberLiteral;
struct BoolLiteral;
struct StringLiteral;
struct Identifier;
struct BinaryExpression;
struct UnaryExpression;
struct FunctionCall;
struct Function;

//statements
struct VariableDeclaration;
struct Return;
struct If;
struct While;
struct VariableAssignment;
struct FunctionCallStatement;


class Visitor
{
public:
    virtual ~Visitor() = 0;

    //expressions
    virtual void visit(NumberLiteral&) = 0;
    virtual void visit(BoolLiteral&) = 0;
    virtual void visit(StringLiteral&) = 0;
    virtual void visit(Identifier&) = 0;
    virtual void visit(BinaryExpression&) = 0;
    virtual void visit(UnaryExpression&) = 0;
    virtual void visit(FunctionCall&) = 0;

    //statements
    virtual void visit(VariableDeclaration&) = 0;
    virtual void visit(Function&) = 0;
    virtual void visit(Return&) = 0;
    virtual void visit(If&) = 0;
    virtual void visit(While&) = 0;
    virtual void visit(VariableAssignment&) = 0;
    virtual void visit(FunctionCallStatement&) = 0;
};

#endif //COMPILER_PROJECT_VISITOR_H