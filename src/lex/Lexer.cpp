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
        } else if (isalpha(current) || current == '_') {
            const auto text = parseIdentifierOrKeyword();

            if (text == "_") {
                token = Token(Underscore(), _line, _col, text);
            } else if (const auto it = keywords.find(text); it != keywords.end()) {
                token = Token{it->second, _line, _col, text};
            } else {
                token = Token(Identifier{text}, _line, _col, text);
            }
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
                        error(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
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
                        token = Token(Operator::LessThan, _line, _col - 1, std::string{current});
                    }
                    break;
                case '>':
                    if (peekNext() == '=') {
                        eat();
                        eat();
                        token = Token(Operator::GreaterEqual, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        token = Token(Operator::GreaterThan, _line, _col - 1, std::string{current});
                    }
                    break;
                case '&':
                    if (peekNext() == '&') {
                        eat();
                        eat();
                        token = Token(Operator::LogicalAnd, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        error(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                case '|':
                    if (peekNext() == '|') {
                        eat();
                        eat();
                        token = Token(Operator::LogicalOr, _line, _col - 1, std::string{current});
                    } else {
                        eat();
                        error(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
                    }
                    break;
                default:
                    eat();
                    error(LexerErrorType::UnexpectedChar, _line, _col - 1, std::string{current});
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
    bool        isFloat = false;

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
                error(LexerErrorType::InvalidNumber, _line, _col + 1, numberStr + '.');
                eat(); // Consume the offending second dot to advance past the error
                break;
            }

            isFloat = true;
        }

        numberStr.push_back(eat());
    }

    if (numberStr.empty() || (numberStr.length() == 1 && numberStr[0] == '.')) {
        error(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
        return Token(IntegerLiteral{0}, _line, startCol, "");
    }

    if (isFloat) {
        // Ensure numbers don't start or end with a bare dot (e.g., .123 or 123.)
        if (numberStr.front() == '.' || numberStr.back() == '.') {
            error(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(FloatLiteral{0.0f}, _line, startCol, numberStr);
        }

        try {
            float floatValue = std::stof(numberStr);
            return Token(FloatLiteral{floatValue}, _line, startCol, numberStr);
        } catch (const std::out_of_range&) {
            error(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(FloatLiteral{0.0f}, _line, startCol, numberStr);
        }
    } else {
        try {
            int intValue = std::stoi(numberStr);
            return Token(IntegerLiteral{intValue}, _line, startCol, numberStr);
        } catch (const std::out_of_range&) {
            error(LexerErrorType::InvalidNumber, _line, startCol, numberStr);
            return Token(IntegerLiteral{0}, _line, startCol, numberStr);
        }
    }
}

std::string Lexer::parseIdentifierOrKeyword() {
    std::string text;
    const auto  startLine = _line;
    const auto  startCol  = _col;

    text.push_back(eat());

    while (isalnum(peek()) || peek() == '_') {
        text.push_back(eat());
    }

    return text;
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
            error(LexerErrorType::UnterminatedComment, _line, _col, comment);
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
            error(LexerErrorType::UnterminatedString, _line, _col, string);
            return Token{};
        }

        string += eat();
    }
    eat(); // eat closing quotes

    return Token(StringLiteral{string}, _line, _col - 1, string);
}

void Lexer::error(const LexerErrorType& type, int line, int col, const std::string& lexeme) {
    _errors.push_back(LexerError(type, line, col, lexeme));
}
