#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include "Token.h"

#include <regex>

class Lexer
{
public:
    Lexer(std::string& src);
    ~Lexer() = default;

    std::vector<TokenType> lex() const;
    static void printTokens(const std::vector<TokenType>& tokens);

private:
    std::string _sourceCode;

    TokenType getKeyword(const std::string& value) const;
};


#endif //COMPILER_PROJECT_LEXER_H