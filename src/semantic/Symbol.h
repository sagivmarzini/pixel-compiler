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
    TypeNode* type;
    Scope* scope;
    bool isConst;
    std::vector<AST::FunctionDeclaration::FunctionParameter> params; // empty unless it's a function

    std::optional<AST::FunctionDeclaration::FunctionParameter> getParameterByName(const std::string& name) {
        const auto it = std::find_if(params.begin(), params.end(),
                                     [name](const AST::FunctionDeclaration::FunctionParameter& param) {
                                         return param.name == name;
                                     });

        if (it == params.end()) return std::nullopt;

        return *it;
    }
};


#endif //COMPILER_PROJECT_SYMBOL_H
