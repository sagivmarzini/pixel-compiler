#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/Lexer.h"


class Compiler
{
public:
    Compiler(std::string source);

    void compile();

    //TODO: add error handler
private:
    std::string _sourceCode;
};


#endif //COMPILER_PROJECT_COMPILER_H