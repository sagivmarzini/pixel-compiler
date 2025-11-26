#include "LexerException.h"

#include <format>
#include <utility>

LexerException::LexerException(const LexerError &type, int line, int col, std::string lexeme)
    : _type(type), _location(line, col), _lexeme(std::move(lexeme)) {
    switch (_type) {
        case LexerError::UnexpectedChar:
            _msg = std::format("Unexpected token '{}'", _lexeme);
            break;
        case LexerError::UnterminatedString:
            _msg = std::format("Unterminated string '{}'", _lexeme);
            break;
        case LexerError::UnterminatedComment:
            _msg = std::format("Unterminated comment '{}'", _lexeme);
            break;
        case LexerError::InvalidNumber:
            _msg = std::format("Invalid number '{}'", _lexeme);
            break;
        default:
            _msg = "Unknown error";
    }
}

const char *LexerException::what() const noexcept {
    return _msg.c_str();
}

TokenLocation LexerException::location() const {
    return _location;
}
