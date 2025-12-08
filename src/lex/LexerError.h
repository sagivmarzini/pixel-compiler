//
// Created by Sagiv Marzini on 21/11/2025.
//

#ifndef COMPILER_PROJECT_LEXERERROR_H
#define COMPILER_PROJECT_LEXERERROR_H

#include "Token.h"

#include <string>

enum class LexerErrorType {
    UnexpectedChar,
    UnterminatedString,
    UnterminatedComment,
    InvalidNumber,
};


class LexerError {
public:
    LexerError(const LexerErrorType &type, int line, int col, std::string lexeme);

    [[nodiscard]] std::string message() const;

    [[nodiscard]] TokenMetadata getMetadata() const;

private:
    std::string _msg;
    LexerErrorType _type;
    TokenMetadata _metadata;
};


#endif //COMPILER_PROJECT_LEXERERROR_H
