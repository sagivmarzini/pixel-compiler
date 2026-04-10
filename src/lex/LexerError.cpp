#include "LexerError.h"

#include <format>

LexerError::LexerError(const LexerErrorType& type, int line, int col, const std::string& lexeme)
    : CompilerError({line, col, lexeme}) {
    switch (type) {
        case LexerErrorType::UnexpectedChar:
            _message = std::format("unexpected character '{}'", lexeme);
            break;
        case LexerErrorType::UnterminatedString:
            _message = "missing terminating '\"' character";
            break;
        case LexerErrorType::UnterminatedComment:
            _message = "unterminated block comment";
            break;
        case LexerErrorType::InvalidNumber:
            _message = std::format("invalid numeric literal '{}'", lexeme);
            break;
        default:
            _message = "unknown lexical error";
    }
}
