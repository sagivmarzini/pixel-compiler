#include "Lexer.h"

#include <utility>
#include <vector>

#include "CompilerException.h"
#include "LexerError.h"

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
            switch (current) {
                case '"':
                    token = parseStringLiteral();
                    break;
                case ';':
                    eat();
                    token = Token(Semicolon{}, _line, _col - 1, std::string{current});
                    break;
                case ':':
                    eat();
                    token = Token(Colon{}, _line, _col - 1, std::string{current});
                    break;
                case ',':
                    eat();
                    token = Token(Comma{}, _line, _col - 1, std::string{current});
                    break;
                case '{':
                    eat();
                    token = Token(LBrace{}, _line, _col - 1, std::string{current});
                    break;
                case '}':
                    eat();
                    token = Token(RBrace{}, _line, _col - 1, std::string{current});
                    break;
                case '(':
                    eat();
                    token = Token(LParen{}, _line, _col - 1, std::string{current});
                    break;
                case ')':
                    eat();
                    token = Token(RParen{}, _line, _col - 1, std::string{current});
                    break;
                case '.':
                    if (peekNext() == '.') {
                        eat();
                        eat();
                        token = Token(DoubleDot{}, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        _errors.emplace_back(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                case '-':
                    if (peekNext() == '>') {
                        eat();
                        eat();
                        token = Token(Arrow{}, _line, _col - 1, std::string{current});
                    } else if (peekNext() == '-') {
                        eat();
                        eat();
                        token = Token(Operator::MinusMinus, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Minus, _line, _col - 1, std::string{current});
                    }
                    break;
                case '+':
                    if (peekNext() == '+') {
                        eat();
                        eat();
                        token = Token(Operator::PlusPlus, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Plus, _line, _col - 1, std::string{current});
                    }
                    break;
                case '*':
                    eat();
                    token = Token(Operator::Star, _line, _col - 1, std::string{current});
                    break;
                case '/':
                    if (peekNext() == '/') {
                        skipSingleLineComment();
                        continue;
                    }
                    if (peekNext() == '*') {
                        skipMultiLineComment();
                        continue;
                    }

                    eat();
                    token = Token(Operator::Slash, _line, _col - 1, std::string{current});

                    break;
                case '=':
                    if (peekNext() == '=') {
                        eat();
                        eat();
                        token = Token(Operator::Equal, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Assignment, _line, _col - 1, std::string{current});
                    }
                    break;
                case '!':
                    if (peekNext() == '=') {
                        eat();
                        eat();
                        token = Token(Operator::NotEqual, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Exclamation, _line, _col - 1, std::string{current});
                    }
                    break;
                case '<':
                    if (peekNext() == '=') {
                        eat();
                        eat();
                        token = Token(Operator::LessEqual, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Less, _line, _col - 1, std::string{current});
                    }
                    break;
                case '>':
                    if (peekNext() == '=') {
                        eat();
                        eat();
                        token = Token(Operator::GreaterEqual, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::Greater, _line, _col - 1, std::string{current});
                    }
                    break;
                case '&':
                    if (peekNext() == '&') {
                        eat();
                        eat();
                        token = Token(Operator::And, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        _errors.emplace_back(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                case '|':
                    if (peekNext() == '|') {
                        eat();
                        eat();
                        token = Token(Operator::Or, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        _errors.emplace_back(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                default:
                    eat();
                    _errors.emplace_back(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
            }
        }

        tokens.push_back(token);
    }

    if (!_errors.empty())
        throw CompilerException(_errors);

    tokens.emplace_back(EndOfFile{}, _line, _col - 1, "EOF");
    return tokens;
}

char Lexer::peek() const {
    if (_position >= _sourceCode.size()) return '\0';
    return _sourceCode[_position];
}

char Lexer::peekNext() const {
    if (_position + 1 >= _sourceCode.size()) return '\0';
    return _sourceCode[_position + 1];
}

char Lexer::eat() {
    if (_position >= _sourceCode.size()) return '\0';
    _col++;
    return _sourceCode[_position++];
}


Token Lexer::parseNumber() {
    std::string numberStr;
    bool isFloat = false;

    // Position tracking for error messages
    const int startCol = _col;

    while (isdigit(peek()) || peek() == '.') {
        const char current = peek();

        if (current == '.') {
            // Check for the ".." range operator. If found, stop consuming the number here.
            if (peekNext() == '.') {
                break;
            }
            if (isFloat) {
                // Error: Found a second dot within the number (e.g., 1.2.3)
                _errors.emplace_back(LexerErrorType::InvalidNumber, _line, _col + 1, numberStr + '.');
                eat(); // Consume the offending second dot to advance past the error
                break;
            }

            isFloat = true;
        }

        numberStr.push_back(eat());
    }

    if (numberStr.empty() || (numberStr.length() == 1 && numberStr[0] == '.')) {
        _errors.emplace_back(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
        return Token(IntegerLiteral{0}, _line, startCol, "");
    }

    if (isFloat) {
        // Ensure numbers don't start or end with a bare dot (e.g., .123 or 123.)
        if (numberStr.front() == '.' || numberStr.back() == '.') {
            _errors.emplace_back(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(FloatLiteral{0.0f}, _line, startCol, numberStr);
        }

        try {
            float floatValue = std::stof(numberStr);
            return Token(FloatLiteral{floatValue}, _line, startCol, numberStr);
        } catch (const std::out_of_range&) {
            _errors.emplace_back(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(FloatLiteral{0.0f}, _line, startCol, numberStr);
        }
    } else {
        try {
            int intValue = std::stoi(numberStr);
            return Token(IntegerLiteral{intValue}, _line, startCol, numberStr);
        } catch (const std::out_of_range&) {
            _errors.emplace_back(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(IntegerLiteral{0}, _line, startCol, numberStr);
        }
    }
}

Token Lexer::parseIdentifierOrKeyword() {
    std::string text;

    // Identifiers and keywords must start with a letter
    if (!isalpha(peek())) _errors.emplace_back(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{peek()});
    text.push_back(eat());

    while (isalnum(peek()) || peek() == '_') {
        text.push_back(eat());
    }

    // Look up in keyword map
    if (const auto it = keywords.find(text); it != keywords.end()) {
        return {it->second, _line, _col - 1, text};
    }

    // Default to identifier
    return Token(Identifier{text}, _line, _col - 1, text);
}

void Lexer::skipSingleLineComment() {
    while (peek() && peek() != '\n')
        eat();
}

void Lexer::skipMultiLineComment() {
    std::string comment;

    while (true) {
        const char currentChar = peek();

        if (!currentChar) {
            _errors.emplace_back(LexerErrorType::UnterminatedComment, _line, _col, comment);
            return;
        }

        if (currentChar == '*' && peekNext() == '/') {
            eat(); // '*'
            eat(); // '/'
            break;
        }

        if (currentChar == '\n') _line++;

        comment += eat();
    }
}

Token Lexer::parseStringLiteral() {
    std::string string;

    eat(); // eat opening quotes
    while (peek() != '"') {
        if (!peek()) {
            _errors.emplace_back(LexerErrorType::UnterminatedString, _line, _col, string);
            return Token{};
        }

        string += eat();
    }
    eat(); // eat closing quotes

    return Token(StringLiteral{string}, _line, _col - 1, string);
}
