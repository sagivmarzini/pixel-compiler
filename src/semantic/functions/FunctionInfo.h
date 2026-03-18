//
// Created by sagiv on 18/03/2026.
//

#ifndef PXL_LANG_FUNCTIONINFO_H
#define PXL_LANG_FUNCTIONINFO_H
#include <vector>

#include "parse/AST/Statement.h"

enum class FunctionKind {
    Api, // Called by the user
    Internal // Called by calls injected by the backend
};

struct FunctionInfo {
    std::vector<AST::FunctionDeclaration::FunctionParameter> params;
    Type returnType;
    std::string llvmName; // actual symbol name (e.g. "sinf")
    bool isVariadic = false;
    FunctionKind kind;
};

#endif //PXL_LANG_FUNCTIONINFO_H
