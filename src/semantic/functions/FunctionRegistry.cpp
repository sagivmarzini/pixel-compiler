//
// Created by sagiv on 18/03/2026.
//

#include "FunctionRegistry.h"

void FunctionRegistry::registerFunction(const std::string& name, FunctionInfo info) {
    _functions[name] = std::move(info);
}

const FunctionInfo* FunctionRegistry::get(const std::string& name) const {
    auto it = _functions.find(name);
    if (it == _functions.end()) return nullptr;
    return &it->second;
}

std::unordered_map<std::string, FunctionInfo> FunctionRegistry::getAllApiFunctions() const {
    std::unordered_map<std::string, FunctionInfo> apiFuncs;
    for (const auto& [name, info]: _functions) {
        if (info.kind == FunctionKind::Api || info.kind == FunctionKind::Intrinsic) {
            apiFuncs[name] = info;
        }
    }
    return apiFuncs;
}

void FunctionRegistry::registerApi(const std::string& name, const FunctionInfo& func) {
    _functions[name] = func;
    _functions[name].kind = FunctionKind::Api;
}

void FunctionRegistry::registerInternal(const std::string& name, const FunctionInfo& func) {
    _functions[name] = func;
    _functions[name].kind = FunctionKind::Internal;
}

void FunctionRegistry::registerIntrinsic(const std::string& name, const FunctionInfo& func,
                                         const llvm::Intrinsic::ID& id) {
    _functions[name] = func;
    _functions[name].kind = FunctionKind::Intrinsic;
    _functions[name].intrinsicId = id;
}
