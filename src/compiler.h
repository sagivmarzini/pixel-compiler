#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/lexer.h"


class compiler {
public:
    explicit compiler(std::string source);

    void compile() const;

    static void printTokens(const std::vector<token> &tokens);

private:
    std::string _sourceCode;
};


#endif //COMPILER_PROJECT_COMPILER_H
