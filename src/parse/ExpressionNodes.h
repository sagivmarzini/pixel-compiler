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

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    double _value;
};


struct BoolLiteral : ExpressionNode
{
    BoolLiteral(bool value) : _value(value) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    bool _value;
};

struct StringLiteral : ExpressionNode
{
    StringLiteral(std::string& value) : _value(value) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _value;
};

struct Identifier : ExpressionNode
{
    Identifier(std::string& name) : _name(name) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
};


struct BinaryExpression : ExpressionNode
{
    BinaryExpression(std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right, TokenType operation) :
        _left(std::move(left)), _right(std::move(right)), _operation(operation) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _left;
    std::unique_ptr<ExpressionNode> _right;
    TokenType _operation;
};

struct UnaryExpression : ExpressionNode
{
    UnaryExpression(std::unique_ptr<ExpressionNode> operand, TokenType operation) :
        _operand(std::move(operand)), _operation(operation)
    {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _operand;
    TokenType _operation;

};

struct FunctionCall : ExpressionNode
{
    FunctionCall(std::string& name) : _name(name) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
};
#endif //COMPILER_PROJECT_EXPRESSIONNODES_H