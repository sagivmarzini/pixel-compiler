#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <utility>
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
    Unspecified, // Inferred type is initially set to `unspecified`

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
    int value;
};

struct FloatLiteral {
    float value;
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

struct Comma {
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
    Comma,
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
    FloatLiteral,
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

struct TokenMetadata {
    int line;
    int col;
    std::string lexeme;
};

struct Token {
    TokenType type;
    TokenMetadata metadata{};

    Token() = default;

    Token(TokenType type, int line, int col, std::string lexeme)
        : type(std::move(type)), metadata(line, col, std::move(lexeme)) {
    }
};


std::string typeToString(Type type);

std::string operatorToString(Operator op);

std::string keywordToString(Keyword keyword);

std::string tokenToString(const Token &token);

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif // COMPILER_PROJECT_TOKEN_H
