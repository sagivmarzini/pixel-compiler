#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <variant>
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
    Assignment,

    Plus,
    Minus,
    Star,
    Slash,

    And,
    Or,
    Equal,
    NotEqual,
    Exclamation,

    Less,
    LessEqual,
    Greater,
    GreaterEqual,
};

// Token variant
using TokenType = std::variant<
    // simple tokens
    struct Semicolon,
    struct Colon,
    struct LBrace,
    struct RBrace,
    struct LParen,
    struct RParen,
    struct Arrow,
    struct TowDots,

    // payload tokens
    Keyword,
    Identifier,
    IntegerLiteral,
    StringLiteral,
    BooleanLiteral,
    Type,
    Operator,

    // EOF
    struct EndOfFile
>;

#endif // COMPILER_PROJECT_TOKEN_H
