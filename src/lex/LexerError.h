#ifndef COMPILER_PROJECT_LEXERERROR_H
#define COMPILER_PROJECT_LEXERERROR_H

#include "Token.h"

#include <string>

#include "CompilerError.h"

enum class LexerErrorType {
    UnexpectedChar,
    UnterminatedString,
    UnterminatedComment,
    InvalidNumber,
};


class LexerError : public CompilerError {
public:
    LexerError(const LexerErrorType& type, int line, int col, const std::string& lexeme);
};


#endif //COMPILER_PROJECT_LEXERERROR_H
