//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SYMBOL_H
#define COMPILER_PROJECT_SYMBOL_H

#include <string>
#include <vector>

#include "lex/Token.h"

class SymbolPool;
class Scope;

struct Symbol {
    enum class SymbolKind {
        Variable,
        Function,
        Parameter,
        TypeDefinition,
        Constant
    };

    std::string name;
    SymbolKind kind;
    Type type;
    Scope* scope;
    std::vector<Type> paramTypes;
};


#endif //COMPILER_PROJECT_SYMBOL_H
