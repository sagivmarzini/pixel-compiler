#ifndef COMPILER_PROJECT_STATEMENTNODE_H
#define COMPILER_PROJECT_STATEMENTNODE_H

#include "AstNode.h"
#include "../lex/Token.h"

#include <vector>
#include <memory>

#include "Visitor.h"


struct BlockNode // stuff containing { } (function, if)
{
    BlockNode()
    {
        body = std::vector<std::unique_ptr<AstNode>>();
    }

    void addNode(std::unique_ptr<AstNode> node)
    {
        body.push_back(std::move(node));
    }
    std::vector<std::unique_ptr<AstNode>> body;

};


struct VariableDeclaration : StatementNode
{
    VariableDeclaration(std::string& name, std::unique_ptr<ExpressionNode> value, std::optional<TokenType> type) :
        _name(name), _value(std::move(value)), _type(type)
    {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
    std::unique_ptr<ExpressionNode> _value;
    std::optional<TokenType> _type; //this is optional bc var type can be inferred
};


struct Function : StatementNode
{
    //TODO: add params!
    Function(std::string& name, TokenType returnType) :
        _name(name), _returnType(returnType)
    {
        _block = BlockNode();
    }

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
    TokenType _returnType;
    BlockNode _block;
};

struct Return : StatementNode
{
    Return(std::unique_ptr<ExpressionNode> value) : _value(std::move(value))
    {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _value;
};

struct If : StatementNode
{
    If(std::unique_ptr<ExpressionNode> condition) : _condition(std::move(condition))
    {
        _block = BlockNode();
    }

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _condition;
    BlockNode _block;
};

/* TODO: add these statements:
 *  else
 *  else if
 *  for
 */

struct While : StatementNode
{
    While(std::unique_ptr<ExpressionNode> condition) : _condition(std::move(condition))
    {
        _block = BlockNode();
    }

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _condition;
    BlockNode _block;
};

struct VariableAssignment : StatementNode
{
    VariableAssignment(std::unique_ptr<ExpressionNode> value, std::string& name) :
    _newValue(std::move(value)), _name(name)
    {
    }

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> _newValue;
    std::string _name;
};


struct FunctionCallStatement : StatementNode
{
    FunctionCallStatement(std::string& name) : _name(name) {}

    void access(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    std::string _name;
};

#endif //COMPILER_PROJECT_STATEMENTNODE_H