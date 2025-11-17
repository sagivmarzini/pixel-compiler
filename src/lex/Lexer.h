#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include <regex>

#include "Token.h"

class Lexer
{
public:
    Lexer(std::string& src);
    ~Lexer() = default;

    std::vector<Token> lex() const;
    static void printTokens(const std::vector<Token>& tokens);

private:
    std::string _sourceCode;

    TokenType getKeyword(const std::string& value) const;
};


#endif //COMPILER_PROJECT_LEXER_H