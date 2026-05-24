#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/Lexer.h"
#include "semantic/functions/FunctionRegistry.h"
#include "semantic/globals/GlobalRegistry.h"
#include "types/TypeContext.h"


class Compiler {
public:
    explicit Compiler(std::string sourceFile);

    void compile();

private:
    std::string _sourceFileName;
    std::string _sourceCode;
    TypeContext _typeContext;

    FunctionRegistry _functionRegistry;
    GlobalRegistry _globalRegistry;

    void initFunctions();

    void initGlobals();
};


#endif //COMPILER_PROJECT_COMPILER_H
