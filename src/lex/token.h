#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <variant>
#include <unordered_map>
#include <iostream>

enum class Keyword {
    Func,
    Var,
    Return,
    If,
    Else,
    While,
    For
};

enum class Type {
    Int,
    Float,
    Bool,
    Ptr,
    String,
    Color,
    Void,
};

// Token payload structs
struct Identifier {
    std::string name;
};

struct IntegerLiteral {
    long value;
};

struct StringLiteral {
    std::string value;
};

struct BooleanLiteral {
    bool value;
};

enum class Operator {
    Assignment, // =

    Plus, // +
    PlusPlus, // ++
    Minus, // -
    MinusMinus, // --
    Star, // *
    Slash, // /

    And, // &&
    Or, // ||
    Equal, // ==
    NotEqual, // !=
    Exclamation, // !

    Less, // <
    LessEqual, // <=
    Greater, // >
    GreaterEqual, // >=
};

struct Semicolon {
};

struct Colon {
};

struct LBrace {
};

struct RBrace {
};

struct LParen {
};

struct RParen {
};

struct Arrow {
};

struct DoubleDot {
};

struct EndOfFile {
};

// Token variant
using TokenType = std::variant<
    // simple tokens
    Semicolon,
    Colon,
    LBrace,
    RBrace,
    LParen,
    RParen,
    Arrow,
    DoubleDot,

    // payload tokens
    Operator,

    Keyword,
    IntegerLiteral,
    StringLiteral,
    BooleanLiteral,
    Type,
    Identifier,

    // EOF
    EndOfFile
>;

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", Keyword::Func},
    {"var", Keyword::Var},
    {"return", Keyword::Return},
    {"if", Keyword::If},
    {"else", Keyword::Else},
    {"while", Keyword::While},
    {"for", Keyword::For},

    {"int", Type::Int},
    {"float", Type::Float},
    {"bool", Type::Bool},
    {"ptr", Type::Ptr},
    {"string", Type::String},
    {"color", Type::Color},
    {"void", Type::Void},

    {"true", BooleanLiteral{true}},
    {"false", BooleanLiteral{false}},
};


struct token {
    TokenType type;
    // std::string lexeme;
    // int line = 0;
    // int column = 0;};
};

inline std::string tokenToString(const token &token) {
    return std::visit([]<typename U>(U &&arg) -> std::string {
        using T = std::decay_t<U>;

        if constexpr (std::is_same_v<T, Semicolon>) return "Semicolon";
        else if constexpr (std::is_same_v<T, Colon>) return "Colon";
        else if constexpr (std::is_same_v<T, LBrace>) return "LBrace";
        else if constexpr (std::is_same_v<T, RBrace>) return "RBrace";
        else if constexpr (std::is_same_v<T, LParen>) return "LParen";
        else if constexpr (std::is_same_v<T, RParen>) return "RParen";
        else if constexpr (std::is_same_v<T, Arrow>) return "Arrow";
        else if constexpr (std::is_same_v<T, DoubleDot>) return "DoubleDot";
        else if constexpr (std::is_same_v<T, EndOfFile>) return "EndOfFile";
        else if constexpr (std::is_same_v<T, Operator>) {
            switch (arg) {
                case Operator::Assignment: return "Operator(=)";
                case Operator::Plus: return "Operator(+)";
                case Operator::PlusPlus: return "Operator(++)";
                case Operator::Minus: return "Operator(-)";
                case Operator::MinusMinus: return "Operator(--)";
                case Operator::Star: return "Operator(*)";
                case Operator::Slash: return "Operator(/)";
                case Operator::And: return "Operator(&&)";
                case Operator::Or: return "Operator(||)";
                case Operator::Equal: return "Operator(==)";
                case Operator::NotEqual: return "Operator(!=)";
                case Operator::Exclamation: return "Operator(!)";
                case Operator::Less: return "Operator(<)";
                case Operator::LessEqual: return "Operator(<=)";
                case Operator::Greater: return "Operator(>)";
                case Operator::GreaterEqual: return "Operator(>=)";
            }
        } else if constexpr (std::is_same_v<T, Keyword>) {
            switch (arg) {
                case Keyword::Func: return "Keyword(func)";
                case Keyword::Var: return "Keyword(var)";
                case Keyword::Return: return "Keyword(return)";
                case Keyword::If: return "Keyword(if)";
                case Keyword::Else: return "Keyword(else)";
                case Keyword::While: return "Keyword(while)";
                case Keyword::For: return "Keyword(for)";
            }
        } else if constexpr (std::is_same_v<T, Type>) {
            switch (arg) {
                case Type::Int: return "Type(int)";
                case Type::Float: return "Type(float)";
                case Type::Bool: return "Type(bool)";
                case Type::Ptr: return "Type(ptr)";
                case Type::String: return "Type(string)";
                case Type::Color: return "Type(color)";
                case Type::Void: return "Type(void)";
            }
        } else if constexpr (std::is_same_v<T, Identifier>) {
            return "Identifier(" + arg.name + ")";
        } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
            return "IntegerLiteral(" + std::to_string(arg.value) + ")";
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            return "StringLiteral(\"" + arg.value + "\")";
        } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
            return arg.value ? "BooleanLiteral(true)" : "BooleanLiteral(false)";
        }

        return "Unknown";
    }, token.type);
}

inline std::ostream &operator<<(std::ostream &os, const token &token) {
    os << tokenToString(token);
    return os;
}

#endif // COMPILER_PROJECT_TOKEN_H
