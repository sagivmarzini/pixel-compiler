#include "ParserError.h"
#include <format>

ParserError::ParserError(const ParserErrorType& type, const Token& errorToken, const TokenType& expectedTokenType)
    : CompilerError(errorToken.metadata) {
    switch (type) {
        case ParserErrorType::MissingClosingBrace:
            _message = "expected '}'";
            break;
        case ParserErrorType::MissingClosingParen:
            _message = "expected ')'";
            break;
        case ParserErrorType::TypelessVarDeclaration:
            _message = "a type specifier or value to infer from is required for all declarations";
            break;
        case ParserErrorType::UnexpectedToken:
            _message = std::format("expected '{}'; found '{}'",
                                   tokenTypeToString(expectedTokenType),
                                   tokenTypeToString(errorToken.type));
            break;
        case ParserErrorType::ExpectedExpression:
            _message = "expected expression";
            break;
        case ParserErrorType::ExpectedDeclaration:
            _message = "expected a declaration";
            break;
        case ParserErrorType::TrailingComma:
            _message = "trailing comma";
            break;
        case ParserErrorType::ElseWithoutIf:
            _message = "'else' statement has no corresponding 'if'";
            break;
        case ParserErrorType::IncrementNonVariable:
            _message = "cannot apply operator ++/-- on non variable types";
            break;
    }
}