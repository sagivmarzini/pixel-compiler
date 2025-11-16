#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <optional>

enum class TokenType
{
    Comment,
    EndOfFile,

    Identifier,

    Literal_Number,
    Literal_True,
    Literal_False,
    Literal_String,

    Type_Int,
    Type_Float,
    Type_Bool,
    Type_Ptr,
    Type_String,
    Type_Color,
    Type_Void,

    Keyword_Func,
    Keyword_Var,
    Keyword_Return,
    Keyword_If,
    Keyword_Else,
    Keyword_While,
    Keyword_For,

    Symbol_Semicolon,
    Symbol_Colon,
    Symbol_LBrace,
    Symbol_RBrace,
    Symbol_LParen,
    Symbol_RParen,
    Symbol_Arrow,
    Symbol_TowDots,

    Operator_Assignment,

    Operator_Plus,
    Operator_Minus,
    Operator_Star,
    Operator_Slash,

    Operator_And,
    Operator_Or,
    Operator_Equal,
    Operator_NotEqual,
    Operator_Exclamation,

    Operator_Less,
    Operator_LessEqual,
    Operator_Greater,
    Operator_GreaterEqual,
};

const std::string& getTokenTypeString(TokenType type);

struct Token
{
    TokenType type;
    std::optional<std::string> value;


    void print(std::ostream& out) const
    {
        if (type == TokenType::Identifier)
            out << "Identifier(" << value.value() << ')';

        else if (type == TokenType::Literal_Number)
            out << "Num(" << value.value() << ')';

        else if (type == TokenType::Literal_String)
            out << "String(" << value.value() << ')';

        else
            out << getTokenTypeString(type);
    }
};

const std::unordered_map<TokenType, std::string> TOKEN_TYPE_TO_NAME =
{
    { TokenType::EndOfFile, "EndOfFile" },

    { TokenType::Identifier, "Identifier" },

    { TokenType::Literal_Number, "Number" },
    { TokenType::Literal_True, "True" },
    { TokenType::Literal_False, "False" },
    { TokenType::Literal_String, "String" },

    { TokenType::Type_Int, "Int" },
    { TokenType::Type_Float, "_Float" },
    { TokenType::Type_Bool, "Bool" },
    { TokenType::Type_Ptr, "Ptr" },
    { TokenType::Type_String, "_String" },
    { TokenType::Type_Color, "Color" },
    { TokenType::Type_Void, "Void" },

    { TokenType::Keyword_Func, "Func" },
    { TokenType::Keyword_Var, "Var" },
    { TokenType::Keyword_Return, "Return" },
    { TokenType::Keyword_If, "If" },
    { TokenType::Keyword_Else, "Else" },
    { TokenType::Keyword_While, "While" },
    { TokenType::Keyword_For, "For" },

    { TokenType::Symbol_Semicolon, "Semicolon" },
    { TokenType::Symbol_Colon, "Colon" },
    { TokenType::Symbol_LBrace, "LBrace" },
    { TokenType::Symbol_RBrace, "RBrace" },
    { TokenType::Symbol_LParen, "LParen" },
    { TokenType::Symbol_RParen, "RParen" },
    { TokenType::Symbol_Arrow, "Arrow" },
    { TokenType::Symbol_TowDots, "TowDots" },

    { TokenType::Operator_Assignment, "Assignment" },

    { TokenType::Operator_Plus, "Plus" },
    { TokenType::Operator_Minus, "Minus" },
    { TokenType::Operator_Star, "Star" },
    { TokenType::Operator_Slash, "Slash" },

    { TokenType::Operator_And, "And" },
    { TokenType::Operator_Or, "Or" },
    { TokenType::Operator_Equal, "Equal" },
    { TokenType::Operator_NotEqual, "NotEqual" },
    { TokenType::Operator_Exclamation, "Exclamation" },

    { TokenType::Operator_Less, "Less" },
    { TokenType::Operator_LessEqual, "LessEqual" },
    { TokenType::Operator_Greater, "Greater" },
    { TokenType::Operator_GreaterEqual, "GreaterEqual" }
};

inline const std::string& getTokenTypeString(TokenType type)
{
    return TOKEN_TYPE_TO_NAME.at(type);
}



#endif //COMPILER_PROJECT_TOKEN_H