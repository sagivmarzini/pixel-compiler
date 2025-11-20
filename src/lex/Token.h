#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <variant>
#include <unordered_map>

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

struct Token {
    TokenType type;
    // std::string lexeme;
    // int line = 0;
    // int column = 0;};
};

#endif // COMPILER_PROJECT_TOKEN_H
