//
// Created by sagiv on 18/03/2026.
//

#ifndef PXL_LANG_FUNCTIONREGISTRY_H
#define PXL_LANG_FUNCTIONREGISTRY_H
#include <string>

#include "FunctionInfo.h"
#include "semantic/DeclarationPassVisitor.h"


class FunctionRegistry {
public:
    void registerFunction(const std::string& name, FunctionInfo info);

    const FunctionInfo* get(const std::string& name) const;

    std::unordered_map<std::string, FunctionInfo> getAllApiFunctions() const;

    void registerApi(const std::string& name, const FunctionInfo& func);

    void registerInternal(const std::string& name, const FunctionInfo& func);

    void registerIntrinsic(const std::string& name, const FunctionInfo& func, const llvm::Intrinsic::ID& id);

private:
    std::unordered_map<std::string, FunctionInfo> _functions;
};


#endif //PXL_LANG_FUNCTIONREGISTRY_H
