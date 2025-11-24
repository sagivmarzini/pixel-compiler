#include "Lexer.h"

#include <utility>
#include <vector>

#include "LexerException.h"

Lexer::Lexer(std::string sourceCode)
    : _sourceCode(std::move(sourceCode)), _position(0), _line(1), _col(1) {
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;

    while (_position < _sourceCode.length()) {
        const auto current = peek();

        if (isspace(current)) {
            if (current == '\n') {
                _line++;
                _col = 0;
            }

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
                //TODO: add string literal token
                case ';':
                    token = Token(Semicolon{}, _line, _col - 1, std::string{current});
                    break;
                case ':':
                    token = Token(Colon{}, _line, _col - 1, std::string{current});
                    break;
                case ',':
                    token = Token(Comma{}, _line, _col - 1, std::string{current});
                    break;
                case '{':
                    token = Token(LBrace{}, _line, _col - 1, std::string{current});
                    break;
                case '}':
                    token = Token(RBrace{}, _line, _col - 1, std::string{current});
                    break;
                case '(':
                    token = Token(LParen{}, _line, _col - 1, std::string{current});
                    break;
                case ')':
                    token = Token(RParen{}, _line, _col - 1, std::string{current});
                    break;
                case '.':
                    if (peek() == '.') {
                        eat();
                        token = Token(DoubleDot{}, _line, _col - 1, std::string{current});
                    } else {
                        throw LexerException(LexerError::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                case '-':
                    if (peek() == '>') {
                        eat();
                        token = Token(Arrow{}, _line, _col - 1, std::string{current});
                    } else if (peek() == '-') {
                        eat();
                        token = Token(Operator::MinusMinus, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Minus, _line, _col - 1, std::string{current});
                    }
                    break;
                case '+':
                    if (peek() == '+') {
                        eat();
                        token = Token(Operator::PlusPlus, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Plus, _line, _col - 1, std::string{current});
                    }
                    break;
                case '*':
                    token = Token(Operator::Star, _line, _col - 1, std::string{current});
                    break;
                case '/':
                    token = Token(Operator::Slash, _line, _col - 1, std::string{current});
                    break;
                case '=':
                    if (peek() == '=') {
                        eat();
                        token = Token(Operator::Equal, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Assignment, _line, _col - 1, std::string{current});
                    }
                    break;
                case '!':
                    if (peek() == '=') {
                        eat();
                        token = Token(Operator::NotEqual, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Exclamation, _line, _col - 1, std::string{current});
                    }
                    break;
                case '<':
                    if (peek() == '=') {
                        eat();
                        token = Token(Operator::LessEqual, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Less, _line, _col - 1, std::string{current});
                    }
                    break;
                case '>':
                    if (peek() == '=') {
                        eat();
                        token = Token(Operator::GreaterEqual, _line, _col - 1, std::string{current});
                    } else {
                        token = Token(Operator::Greater, _line, _col - 1, std::string{current});
                    }
                    break;
                case '&':
                    if (peek() == '&') {
                        eat();
                        token = Token(Operator::And, _line, _col - 1, std::string{current});
                    } else {
                        throw LexerException(LexerError::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                case '|':
                    if (peek() == '|') {
                        eat();
                        token = Token(Operator::Or, _line, _col - 1, std::string{current});
                    } else {
                        throw LexerException(LexerError::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                default:
                    throw LexerException(LexerError::UnexpectedChar, _line, _col - 1, std::string{current});
            }
        }

        tokens.push_back(token);
    }

    tokens.push_back(Token(EndOfFile{}, _line, _col - 1, "EOF"));
    return tokens;
}

char Lexer::peek() const {
    if (_position >= _sourceCode.size()) return '\0';
    return _sourceCode[_position];
}

char Lexer::eat() {
    _col++;
    return _sourceCode[_position++];
}

Token Lexer::parseNumber() {
    std::string numberStr;
    int number = 0;

    while (isdigit(peek())) {
        numberStr.push_back(eat());
    }

    if (numberStr.empty()) throw std::runtime_error("There was no number to parse.");

    try {
        number = std::stoi(numberStr);
    } catch (const std::out_of_range &) {
        throw LexerException(LexerError::InvalidNumber, _line, _col - 1, numberStr);
    }

    return Token(IntegerLiteral{number}, _line, _col - 1, numberStr);
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
        return Token(it->second, _line, _col - 1, text);
    }

    // Default to identifier
    return Token(Identifier{text}, _line, _col - 1, text);
}

