#include "LexerError.h"

#include <format>
#include <utility>

LexerError::LexerError(const LexerErrorType &type, int line, int col, std::string lexeme)
    : _type(type), _metadata(line, col, std::move(lexeme)) {
    switch (_type) {
        case LexerErrorType::UnexpectedChar:
            _msg = std::format("unexpected token '{}'", _metadata.lexeme);
            break;
        case LexerErrorType::UnterminatedString:
            _msg = std::format("unterminated string '{}'", _metadata.lexeme);
            break;
        case LexerErrorType::UnterminatedComment:
            _msg = std::format("unterminated comment '{}'", _metadata.lexeme);
            break;
        case LexerErrorType::InvalidNumber:
            _msg = std::format("invalid number '{}'", _metadata.lexeme);
            break;
        default:
            _msg = "unknown error";
    }
}

std::string LexerError::message() const {
    return _msg;
}

TokenMetadata LexerError::getMetadata() const {
    return _metadata;
}
