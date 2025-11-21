#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/Lexer.h"


class Compiler {
public:
    // TODO: Change it so the compiler takes a file name.
    explicit Compiler(std::string sourceCode);

    void compile() const;

    static void printTokens(const std::vector<Token> &tokens);

private:
    std::string _sourceCode;
};


#endif //COMPILER_PROJECT_COMPILER_H
