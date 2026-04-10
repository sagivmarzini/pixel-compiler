//
// Created by sagiv on 18/03/2026.
//

#ifndef PXL_LANG_FUNCTIONINFO_H
#define PXL_LANG_FUNCTIONINFO_H
#include <vector>
#include <llvm/IR/Function.h>

#include "parse/AST/Statement.h"

enum class FunctionKind {
    Api, // Called by the user
    Internal, // Called by calls injected by the backend
    Intrinsic // LLVM intrinsic
};

struct FunctionInfo {
    std::vector<AST::FunctionDeclaration::FunctionParameter> params;
    TypeNode* returnType;
    std::string llvmName; // actual symbol name (e.g. "sinf")
    bool isVariadic = false;
    FunctionKind kind;
    std::optional<llvm::Intrinsic::ID> intrinsicId; // set if kind == Intrinsic
};

#endif //PXL_LANG_FUNCTIONINFO_H
