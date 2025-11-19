#include "Lexer.h"

#include <vector>

Lexer::Lexer(const std::string &sourceCode)
    : _sourceCode(sourceCode), _position(0) {
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;

    while (_position < _sourceCode.length()) {
        const auto current = eat();
        Token token;

        if (isspace(current)) {
            ++_position;
            continue;
        }

        if (isdigit(current)) {
            parseNumber();
        } else if (isalpha(current)) {
            parseIdentifierOrKeyword();
        } else {
            switch (current) {
                case ';':
                    token = makeToken(Semicolon{});
                    break;
                    // etc. for all the other basic tokens
            }
        }
    }

    return tokens;
}

char Lexer::peek() const {
    return _sourceCode[_position];
}

char Lexer::eat() {
    return _sourceCode[_position++];
}
