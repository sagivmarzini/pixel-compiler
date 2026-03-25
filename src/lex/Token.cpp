#include "Token.h"

#include <stdexcept>

std::ostream& operator<<(std::ostream& os, const Keyword& keyword) {
    return os << keywordToString(keyword);
}

std::ostream& operator<<(std::ostream& os, const PrimitiveKind& type) {
    return os << typeToString(type);
}

std::ostream& operator<<(std::ostream& os, const Operator& op) {
    return os << operatorToString(op);
}

Token::Token(TokenType type, int line, int col, std::string lexeme)
    : type(std::move(type)), metadata(line, col, std::move(lexeme)) {
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << tokenTypeToString(token.type);
    return os;
}

std::string typeToString(const PrimitiveKind type) {
    switch (type) {
        case PrimitiveKind::Int: return "int";
        case PrimitiveKind::Float: return "float";
        case PrimitiveKind::Bool: return "boolean";
        case PrimitiveKind::Pointer: return "ptr";
        case PrimitiveKind::String: return "string";
        case PrimitiveKind::Color: return "color";
        case PrimitiveKind::Void: return "void";
        case PrimitiveKind::Unspecified: return "Type"; // for printing 'expected a 'type' token'
        case PrimitiveKind::Error: return "Error";
        default: return "unknown type";
    }
}

std::string operatorToString(Operator op) {
    switch (op) {
        case Operator::Assignment: return "=";
        case Operator::Plus: return "+";
        case Operator::PlusPlus: return "++";
        case Operator::Minus: return "-";
        case Operator::MinusMinus: return "--";
        case Operator::Star: return "*";
        case Operator::Slash: return "/";
        case Operator::LogicalAnd: return "&&";
        case Operator::LogicalOr: return "||";
        case Operator::Equal: return "==";
        case Operator::NotEqual: return "!=";
        case Operator::Exclamation: return "!";
        case Operator::LessThan: return "<";
        case Operator::LessEqual: return "<=";
        case Operator::GreaterThan: return ">";
        case Operator::GreaterEqual: return ">=";
        default: return "unknown operator";
    }
}

std::string keywordToString(Keyword keyword) {
    switch (keyword) {
        case Keyword::Func: return "Func";
        case Keyword::Var: return "Var";
        case Keyword::Const: return "Const";
        case Keyword::Return: return "Return";
        case Keyword::If: return "If";
        case Keyword::Else: return "Else";
        case Keyword::While: return "While";
        case Keyword::For: return "For";
        case Keyword::In: return "In";
        case Keyword::Step: return "Step";
        default: throw std::runtime_error("Print function not implemented for this keyword");
    }
}

std::string tokenTypeToString(const TokenType& tokenType) {
    return std::visit([]<typename U>(U&& arg) -> std::string {
        using T = std::decay_t<U>;

        if constexpr (std::is_same_v<T, Semicolon>) return ";";
        else if constexpr (std::is_same_v<T, Comma>) return ",";
        else if constexpr (std::is_same_v<T, Colon>) return ":";
        else if constexpr (std::is_same_v<T, LeftBrace>) return "{";
        else if constexpr (std::is_same_v<T, RightBrace>) return "}";
        else if constexpr (std::is_same_v<T, LeftParen>) return "(";
        else if constexpr (std::is_same_v<T, RightParen>) return ")";
        else if constexpr (std::is_same_v<T, LeftBracket>) return "[";
        else if constexpr (std::is_same_v<T, RightBracket>) return "]";
        else if constexpr (std::is_same_v<T, Arrow>) return "->";
        else if constexpr (std::is_same_v<T, DoubleDot>) return "..";
        else if constexpr (std::is_same_v<T, Underscore>) return "Underscore";
        else if constexpr (std::is_same_v<T, EndOfFile>) return "EndOfFile";
        else if constexpr (std::is_same_v<T, Operator>) {
            return operatorToString(arg);
        } else if constexpr (std::is_same_v<T, Keyword>) {
            return keywordToString(arg);
        } else if constexpr (std::is_same_v<T, PrimitiveKind>) {
            return typeToString(arg);
        } else if constexpr (std::is_same_v<T, Identifier>) {
            return "Identifier(" + arg.name + ")";
        } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
            return "IntegerLiteral" + (arg.value == -1 ? "" : "(" + std::to_string(arg.value) + ")");
        } else if constexpr (std::is_same_v<T, FloatLiteral>) {
            return "FloatLiteral(" + std::to_string(arg.value) + ")";
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            return "StringLiteral(\"" + arg.value + "\")";
        } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
            return arg.value ? "BooleanLiteral(true)" : "BooleanLiteral(false)";
        }

        throw std::runtime_error("Print not implemented for this token");
    }, tokenType);
}


