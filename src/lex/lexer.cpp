#include "lexer.h"

#include <utility>
#include <vector>

lexer::lexer(std::string sourceCode)
    : _sourceCode(std::move(sourceCode)), _position(0) {
}

std::vector<token> lexer::lex() {
    std::vector<token> tokens;

    while (_position < _sourceCode.length()) {
        const auto current = peek();

        if (isspace(current)) {
            eat();
            continue;
        }

        token token;

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
                        // TODO: Replace with dedicated lexer exception class
                        throw std::runtime_error(std::format("Invalid token: {}", current));
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
                        // TODO: Replace with dedicated lexer exception class
                        throw std::runtime_error(std::format("Invalid token: {}", current));
                    }
                    break;
                case '|':
                    if (peek() == '|') {
                        eat();
                        token = makeToken(Operator::Or);
                    } else {
                        // TODO: Replace with dedicated lexer exception class
                        throw std::runtime_error(std::format("Invalid token: {}", current));
                    }
                    break;
                default:
                    // TODO: Replace with dedicated lexer exception class
                    throw std::runtime_error(std::format("Invalid token: {}", current));
            }
        }

        tokens.push_back(token);
    }

    tokens.push_back(makeToken(EndOfFile{}));
    return tokens;
}

char lexer::peek() const {
    return _sourceCode[_position];
}

char lexer::eat() {
    return _sourceCode[_position++];
}

token lexer::parseNumber() {
    std::string numberStr;

    while (isdigit(peek())) {
        numberStr.push_back(eat());
    }

    if (numberStr.empty()) throw std::runtime_error("There was no number to parse.");

    return makeToken(IntegerLiteral{std::stol(numberStr)});
}

token lexer::parseIdentifierOrKeyword() {
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

token lexer::makeToken(const TokenType &type) {
    return token{type};
}
