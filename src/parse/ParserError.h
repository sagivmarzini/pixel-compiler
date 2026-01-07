//
// Created by Sagiv Marzini on 02/01/2026.
//

#ifndef COMPILER_PROJECT_PARSERERROR_H
#define COMPILER_PROJECT_PARSERERROR_H
#include "CompilerError.h"

enum class ParserErrorType {
    MissingClosingBrace,
    MissingClosingParen,
    TypelessVarDeclaration,
    UnexpectedToken,
    ExpectedExpression,
    ExpectedDeclaration,
    TrailingComma,
    ElseWithoutIf,
};


class ParserError : public CompilerError {
public:
    ParserError(const ParserErrorType& type, const Token& errorToken, const TokenType& expectedTokenType);
};


#endif //COMPILER_PROJECT_PARSERERROR_H
