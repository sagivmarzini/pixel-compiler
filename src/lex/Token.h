#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <optional>
#include <unordered_map>

enum class TokenType
{
    Comment,
    EndOfFile,

    Identifier,

    LiteralNumber,
    LiteralTrue,
    LiteralFalse,
    LiteralString,

    TypeInt,
    TypeFloat,
    TypeBool,
    TypePtr,
    TypeString,
    TypeColor,
    TypeVoid,

    KeywordFunc,
    KeywordVar,
    KeywordReturn,
    KeywordIf,
    KeywordElse,
    KeywordWhile,
    KeywordFor,

    SymbolSemicolon,
    SymbolColon,
    SymbolComma,
    SymbolLBrace,
    SymbolRBrace,
    SymbolLParen,
    SymbolRParen,
    SymbolArrow,
    SymbolDoublePeriod,

    OperatorAssignment,

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

const std::string& getTokenTypeString(TokenType type);

struct Token
{
    TokenType type;
    std::optional<std::string> value;


    friend std::ostream& operator<<(std::ostream& out, const Token& token)
    {
        if (token.type == TokenType::Identifier)
            out << "Identifier(" << token.value.value() << ')';

        else if (token.type == TokenType::LiteralNumber)
            out << "Num(" << token.value.value() << ')';

        else if (token.type == TokenType::LiteralString)
            out << "String(" << token.value.value() << ')';

        else
            out << getTokenTypeString(token.type);
        return out;
    }
};

const std::unordered_map<TokenType, std::string> TOKEN_TYPE_TO_NAME =
{
    { TokenType::EndOfFile, "EndOfFile" },

    { TokenType::Identifier, "Identifier" },

    { TokenType::LiteralNumber, "Number" },
    { TokenType::LiteralTrue, "True" },
    { TokenType::LiteralFalse, "False" },
    { TokenType::LiteralString, "String" },

    { TokenType::TypeInt, "Int" },
    { TokenType::TypeFloat, "Float" },
    { TokenType::TypeBool, "Bool" },
    { TokenType::TypePtr, "Ptr" },
    { TokenType::TypeString, "String" },
    { TokenType::TypeColor, "Color" },
    { TokenType::TypeVoid, "Void" },

    { TokenType::KeywordFunc, "Func" },
    { TokenType::KeywordVar, "Var" },
    { TokenType::KeywordReturn, "Return" },
    { TokenType::KeywordIf, "If" },
    { TokenType::KeywordElse, "Else" },
    { TokenType::KeywordWhile, "While" },
    { TokenType::KeywordFor, "For" },

    { TokenType::SymbolSemicolon, "Semicolon" },
    { TokenType::SymbolColon, "Colon" },
    {TokenType::SymbolComma, "Comma"},
    { TokenType::SymbolLBrace, "LBrace" },
    { TokenType::SymbolRBrace, "RBrace" },
    { TokenType::SymbolLParen, "LParen" },
    { TokenType::SymbolRParen, "RParen" },
    { TokenType::SymbolArrow, "Arrow" },
    { TokenType::SymbolDoublePeriod, "DoublePeriod"},

    { TokenType::OperatorAssignment, "Assignment" },

    { TokenType::OperatorPlus, "Plus" },
    { TokenType::OperatorMinus, "Minus" },
    { TokenType::OperatorStar, "Star" },
    { TokenType::OperatorSlash, "Slash" },

    { TokenType::OperatorAnd, "And" },
    { TokenType::OperatorOr, "Or" },
    { TokenType::OperatorEqual, "Equal" },
    { TokenType::OperatorNotEqual, "NotEqual" },
    { TokenType::OperatorExclamation, "Exclamation" },

    { TokenType::OperatorLess, "Less" },
    { TokenType::OperatorLessEqual, "LessEqual" },
    { TokenType::OperatorGreater, "Greater" },
    { TokenType::OperatorGreaterEqual, "GreaterEqual" }
};

inline const std::string& getTokenTypeString(TokenType type)
{
    return TOKEN_TYPE_TO_NAME.at(type);
}



#endif //COMPILER_PROJECT_TOKEN_H