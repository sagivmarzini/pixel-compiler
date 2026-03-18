#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/Lexer.h"
#include "semantic/functions/FunctionRegistry.h"


class Compiler {
public:
    explicit Compiler(std::string sourceFile);

    void compile() const;

    static void printTokens(const std::vector<Token>& tokens);

private:
    std::string _sourceFileName;
    std::string _sourceCode;
    FunctionRegistry _functionRegistry;

    void initFunctions();
};


#endif //COMPILER_PROJECT_COMPILER_H
