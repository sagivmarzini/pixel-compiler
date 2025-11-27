//
// Created by Sagiv Marzini on 21/11/2025.
//

#ifndef COMPILER_PROJECT_LEXEREXCEPTION_H
#define COMPILER_PROJECT_LEXEREXCEPTION_H

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

    [[nodiscard]] TokenLocation location() const;

private:
    std::string _msg;
    LexerErrorType _type;
    TokenLocation _location{};
    std::string _lexeme;
};


#endif //COMPILER_PROJECT_LEXEREXCEPTION_H
