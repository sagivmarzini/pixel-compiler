#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

std::unique_ptr<ASTNode> Parser::parse() {
    for (auto &token: _tokens) {
        parseStatement();
    }
}

Token &Parser::current() {
    return _tokens[_position];
}

Token &Parser::peek(int offset) {
    if (_tokens.size() <= _position + offset) {
        return _tokens.back(); //return last token if passed the end
    }
    return _tokens[_position + offset];
}

bool Parser::match(TokenType type) {
    return std::visit([&](auto &&value) -> bool {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, TokenType>) {
            if (std::holds_alternative<decltype(value)>(peek().type)) {
                return true;
            }
        }
        return false;
    }, type);
}

Token Parser::expect(TokenType type) {
    if (match(type)) {
        Token token = _tokens[_position];
        _position += 1;
        return token;
    }
    throw std::runtime_error("Unexpected Token! got '" + tokenToString(peek()) + "'");
}

bool Parser::isAtEnd() {
    return match(TokenType(EndOfFile()));
}
