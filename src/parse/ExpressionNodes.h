#ifndef COMPILER_PROJECT_EXPRESSIONNODES_H
#define COMPILER_PROJECT_EXPRESSIONNODES_H

#include "AstNode.h"
#include "../lex/Token.h"

#include "Visitor.h"
#include <string>
#include <memory>



struct NumberLiteral : ExpressionNode
{
    NumberLiteral(double value) : _value(value) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    double _value;
};


struct BoolLiteral : ExpressionNode
{
    BoolLiteral(bool value) : _value(value) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    bool _value;
};

struct StringLiteral : ExpressionNode
{
    StringLiteral(std::string& value) : _value(value) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _value;
};

struct Variable : ExpressionNode
{
    Variable(std::string& name) : _name(name) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
};


struct BinaryExpression : ExpressionNode
{
    BinaryExpression(std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right, Operator operation) :
        _left(std::move(left)), _right(std::move(right)), _operator(operation) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _left;
    std::unique_ptr<ExpressionNode> _right;
    Operator _operator;
};

struct UnaryExpression : ExpressionNode
{
    UnaryExpression(std::unique_ptr<ExpressionNode> operand, Operator operation) :
        _operand(std::move(operand)), _operator(operation)
    {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _operand;
    Operator _operator;

};

struct FunctionCall : ExpressionNode
{
    FunctionCall(const std::string& callee) : _callee(callee) {}

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _callee;
};
#endif //COMPILER_PROJECT_EXPRESSIONNODES_H