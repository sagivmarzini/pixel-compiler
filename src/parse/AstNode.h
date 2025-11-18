#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "../lex/Token.h"

class Visitor; //forward declaration to solve circular include

struct AstNode
{
    virtual ~AstNode() = 0;

    virtual void accept(Visitor&) = 0;
};

struct ExpressionNode : AstNode {};

struct StatementNode : AstNode {};

enum class Operator
{
    //make tokens operators and these operators the same number so conversion is easier
    OperatorAssignment = (int)TokenType::OperatorAssignment,

    OperatorPlus,
    OperatorMinus,
    OperatorStar,
    OperatorSlash,

    OperatorAnd,
    OperatorOr,
    OperatorEqual,
    OperatorNotEqual,
    OperatorExclamation,

    OperatorLess,
    OperatorLessEqual,
    OperatorGreater,
    OperatorGreaterEqual,
};

enum class BaseType
{
    TypeInt = (int)TokenType::TypeInt,
    TypeFloat,
    TypeBool,
    TypePtr,
    TypeString,
    TypeColor,
    TypeVoid,
};



#endif //COMPILER_PROJECT_ASTNODE_H