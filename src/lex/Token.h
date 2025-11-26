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
    Unspecified, //if initializing with inferred type set it to unspecified (like var x = 5;)

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

struct TokenLocation {
    int line;
    int col;
};

struct Token {
    TokenType type;
    TokenLocation location;
    std::string lexeme;

    Token() = default;

    Token(const TokenType &type, int line, int col, const std::string &lexeme)
        : type(type), location(line, col), lexeme(lexeme) {
    }
};


inline std::string typeToString(Type type) {
    switch (type) {
        case Type::Int: return "Int";
        case Type::Float: return "Float";
        case Type::Bool: return "Bool";
        case Type::Ptr: return "Ptr";
        case Type::String: return "String";
        case Type::Color: return "Color";
        case Type::Void: return "Void";
    }
    return "Unknown Type";
}

inline std::string operatorToString(Operator op) {
    switch (op) {
        case Operator::Assignment: return "=";
        case Operator::Plus: return "+";
        case Operator::PlusPlus: return "++";
        case Operator::Minus: return "-";
        case Operator::MinusMinus: return "--";
        case Operator::Star: return "*";
        case Operator::Slash: return "/";
        case Operator::And: return "&&";
        case Operator::Or: return "||";
        case Operator::Equal: return "==";
        case Operator::NotEqual: return "!=";
        case Operator::Exclamation: return "!";
        case Operator::Less: return "<";
        case Operator::LessEqual: return "<=";
        case Operator::Greater: return ">";
        case Operator::GreaterEqual: return ">=";
    }
    return "Unknown Operator";
}

inline std::string keywordToString(Keyword keyword) {
    switch (keyword) {
        case Keyword::Func: return "Func";
        case Keyword::Var: return "Var";
        case Keyword::Return: return "Return";
        case Keyword::If: return "If";
        case Keyword::Else: return "Else";
        case Keyword::While: return "While";
        case Keyword::For: return "For";
    }
    return "Unknown Keyword";
}

inline std::string tokenToString(const Token &token) {
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
            return operatorToString(arg);
        } else if constexpr (std::is_same_v<T, Keyword>) {
            return keywordToString(arg);
        } else if constexpr (std::is_same_v<T, Type>) {
            return typeToString(arg);
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

inline std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << tokenToString(token);
    return os;
}

#endif // COMPILER_PROJECT_TOKEN_H
