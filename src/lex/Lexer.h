#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include "Token.h"

#include <regex>
#include <unordered_map>

#include "LexerError.h"

class Lexer {
public:
    explicit Lexer(std::string sourceCode);

    ~Lexer() = default;


    [[nodiscard]] std::vector<Token> lex();

private:
    std::string _sourceCode;
    size_t _position;
    int _line;
    int _col;
    std::vector<CompilerError> _errors;

    [[nodiscard]] char peek() const;

    [[nodiscard]] char peekNext() const;

    char eat();

    Token parseNumber();


    std::string parseIdentifierOrKeyword();

    void skipSingleLineComment();

    void skipMultiLineComment();

    Token parseStringLiteral();

    static std::string processEscapeSequences(const std::string& input);

    void error(const LexerErrorType& type, int line, int col, const std::string& lexeme);
};


#endif //COMPILER_PROJECT_LEXER_H
