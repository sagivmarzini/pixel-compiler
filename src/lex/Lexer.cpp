#include "Lexer.h"

#include <utility>
#include <vector>

Lexer::Lexer(std::string sourceCode)
    : _sourceCode(std::move(sourceCode)), _position(0) {
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;

    while (_position < _sourceCode.length()) {
        const auto current = peek();

        if (isspace(current)) {
            eat();
            continue;
        }

        Token token;

        if (isdigit(current)) {
            token = parseNumber();
        } else if (isalpha(current)) {
            token = parseIdentifierOrKeyword();
        } else {
            eat(); // Consume the character
            switch (current) {
                case ';':
                    token = makeToken(Semicolon{});
                    break;
                case ':':
                    token = makeToken(Colon{});
                    break;
                case '{':
                    token = makeToken(LBrace{});
                    break;
                case '}':
                    token = makeToken(RBrace{});
                    break;
                case '(':
                    token = makeToken(LParen{});
                    break;
                case ')':
                    token = makeToken(RParen{});
                    break;
                case '.':
                    if (peek() == '.') {
                        eat();
                        token = makeToken(DoubleDot{});
                    } else {
                        // TODO: Handle single dot error
                        throw std::runtime_error("Invalid char " + current);
                    }
                    break;
                case '-':
                    if (peek() == '>') {
                        eat();
                        token = makeToken(Arrow{});
                    } else if (peek() == '-') {
                        eat();
                        token = makeToken(Operator::MinusMinus);
                    } else {
                        token = makeToken(Operator::Minus);
                    }
                    break;
                case '+':
                    if (peek() == '+') {
                        eat();
                        token = makeToken(Operator::PlusPlus);
                    } else {
                        token = makeToken(Operator::Plus);
                    }
                    break;
                case '*':
                    token = makeToken(Operator::Star);
                    break;
                case '/':
                    token = makeToken(Operator::Slash);
                    break;
                case '=':
                    if (peek() == '=') {
                        eat();
                        token = makeToken(Operator::Equal);
                    } else {
                        token = makeToken(Operator::Assignment);
                    }
                    break;
                case '!':
                    if (peek() == '=') {
                        eat();
                        token = makeToken(Operator::NotEqual);
                    } else {
                        token = makeToken(Operator::Exclamation);
                    }
                    break;
                case '<':
                    if (peek() == '=') {
                        eat();
                        token = makeToken(Operator::LessEqual);
                    } else {
                        token = makeToken(Operator::Less);
                    }
                    break;
                case '>':
                    if (peek() == '=') {
                        eat();
                        token = makeToken(Operator::GreaterEqual);
                    } else {
                        token = makeToken(Operator::Greater);
                    }
                    break;
                case '&':
                    if (peek() == '&') {
                        eat();
                        token = makeToken(Operator::And);
                    } else {
                        // TODO: Handle single & error
                        throw std::runtime_error("Invalid char " + current);
                    }
                    break;
                case '|':
                    if (peek() == '|') {
                        eat();
                        token = makeToken(Operator::Or);
                    } else {
                        // TODO: Handle single | error
                        throw std::runtime_error("Invalid char " + current);
                    }
                    break;
                default:
                    // TODO: Unknown character - error
                    throw std::runtime_error("Invalid char " + current);
            }
        }

        tokens.push_back(token);
    }

    tokens.push_back(makeToken(EndOfFile{}));
    return tokens;
}

char Lexer::peek() const {
    return _sourceCode[_position];
}

char Lexer::eat() {
    return _sourceCode[_position++];
}

Token Lexer::parseNumber() {
    std::string numberStr;

    while (isdigit(peek())) {
        numberStr.push_back(eat());
    }

    if (numberStr.empty()) throw std::runtime_error("There was no number to parse.");

    return makeToken(IntegerLiteral{std::stol(numberStr)});
}

Token Lexer::parseIdentifierOrKeyword() {
    std::string text;

    // Identifiers and keywords must start with a letter
    if (!isalpha(peek())) throw std::runtime_error("There was no identifier or keyword to parse.");
    text.push_back(eat());

    while (isalnum(peek()) || peek() == '_') {
        text.push_back(eat());
    }

    // Look up in keyword map
    if (const auto it = keywords.find(text); it != keywords.end()) {
        return makeToken(it->second);
    }

    // Default to identifier
    return makeToken(Identifier{text});
}

Token Lexer::makeToken(const TokenType &type) {
    return Token{type};
}
