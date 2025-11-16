#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include <iostream>

#include "lex/Lexer.h"

class Compiler
{
public:
    Compiler(std::string source);

    void compile();

    //TODO: add error handler
private:
    std::string source;
};


#endif //COMPILER_PROJECT_COMPILER_H