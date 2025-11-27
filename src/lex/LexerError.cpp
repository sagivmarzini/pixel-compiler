#include "LexerError.h"

#include <format>
#include <utility>

LexerError::LexerError(const LexerErrorType &type, int line, int col, std::string lexeme)
    : _type(type), _location(line, col), _lexeme(std::move(lexeme)) {
    switch (_type) {
        case LexerErrorType::UnexpectedChar:
            _msg = std::format("unexpected token '{}'", _lexeme);
            break;
        case LexerErrorType::UnterminatedString:
            _msg = std::format("unterminated string '{}'", _lexeme);
            break;
        case LexerErrorType::UnterminatedComment:
            _msg = std::format("unterminated comment '{}'", _lexeme);
            break;
        case LexerErrorType::InvalidNumber:
            _msg = std::format("invalid number '{}'", _lexeme);
            break;
        default:
            _msg = "unknown error";
    }
}

std::string LexerError::message() const {
    return _msg;
}

TokenLocation LexerError::location() const {
    return _location;
}
