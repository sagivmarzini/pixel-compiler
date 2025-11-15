#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include <string>
#include <iostream>

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