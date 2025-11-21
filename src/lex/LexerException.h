//
// Created by Sagiv Marzini on 21/11/2025.
//

#ifndef COMPILER_PROJECT_LEXEREXCEPTION_H
#define COMPILER_PROJECT_LEXEREXCEPTION_H

#include "CompilerException.h"
#include "Token.h"

enum class LexerError {
    UnexpectedChar,
    UnterminatedString,
    UnterminatedComment,
    InvalidNumber,
};


class LexerException : public CompilerException {
public:
    LexerException(const LexerError &type, int line, int col, std::string lexeme);

    [[nodiscard]] const char *what() const noexcept override;

    [[nodiscard]] TokenLocation location() const;

private:
    LexerError _type;
    TokenLocation _location{};
    std::string _lexeme;
};


#endif //COMPILER_PROJECT_LEXEREXCEPTION_H
