#ifndef COMPILER_PROJECT_STATEMENTNODE_H
#define COMPILER_PROJECT_STATEMENTNODE_H

#include "ExpressionNodes.h"
#include <vector>

struct BlockNode // stuff containing { } (function, if)
{
    void addNode(std::unique_ptr<AstNode> node);

    std::vector<std::unique_ptr<AstNode>> body;
};

//root node- program
struct Program
{
    BlockNode _body;
};

struct VariableDeclaration : StatementNode
{
    VariableDeclaration(const std::string& name, std::unique_ptr<ExpressionNode> value, std::optional<BaseType> type) :
        _variableName(name), _value(std::move(value)), _type(type) {}

    void accept(Visitor& visitor) override;

    std::string _variableName;
    std::unique_ptr<ExpressionNode> _value;
    std::optional<BaseType> _type; //this is optional bc var type can be inferred
};


struct Function : StatementNode
{
    Function(std::string& name, BaseType returnType) :
        _funcName(name), _returnType(returnType) {}

    void accept(Visitor& visitor) override;

    std::string _funcName;
    BaseType _returnType;
    BlockNode _body;
};

struct Return : StatementNode
{
    Return(std::unique_ptr<ExpressionNode> value) :
        _value(std::move(value)) {}

    void accept(Visitor& visitor) override;

    std::unique_ptr<ExpressionNode> _value;
};

struct If : StatementNode
{
    If(std::unique_ptr<ExpressionNode> condition) :
        _condition(std::move(condition)) {}

    void accept(Visitor& visitor) override;

    std::unique_ptr<ExpressionNode> _condition;
    BlockNode _body;
};

struct While : StatementNode
{
    While(std::unique_ptr<ExpressionNode> condition) :
        _condition(std::move(condition)) {}

    void accept(Visitor& visitor) override;

    std::unique_ptr<ExpressionNode> _condition;
    BlockNode _body;
};

struct VariableAssignment : StatementNode
{
    VariableAssignment(std::unique_ptr<ExpressionNode> value, std::string& name) :
    _newValue(std::move(value)), _variableName(name) {}

    void accept(Visitor& visitor) override;

    std::unique_ptr<ExpressionNode> _newValue;
    std::string _variableName;
};


struct FunctionCallStatement : StatementNode
{
    FunctionCallStatement(std::string& name) : _funcName(name) {}

    void accept(Visitor& visitor) override;

    std::string _funcName;
};

#endif //COMPILER_PROJECT_STATEMENTNODE_H