//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SYMBOL_H
#define COMPILER_PROJECT_SYMBOL_H

#include <string>
#include <vector>

#include "lex/Token.h"
#include "parse/AST/Statement.h"

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
    bool isConst;
    std::vector<FunctionDeclaration::FunctionParameter> params; // empty unless it's a function

    std::optional<FunctionDeclaration::FunctionParameter> getParameterByName(const std::string& name) {
        const auto it = std::find_if(params.begin(), params.end(),
                                     [name](const FunctionDeclaration::FunctionParameter& param) {
                                         return param.name == name;
                                     });

        if (it == params.end()) return std::nullopt;

        return *it;
    }
};


#endif //COMPILER_PROJECT_SYMBOL_H
