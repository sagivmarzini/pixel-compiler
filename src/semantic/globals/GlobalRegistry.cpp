//
// Created by sagiv on 20/03/2026.
//

#include "GlobalRegistry.h"

#include "types/TypeNode.h"

void GlobalRegistry::registerGlobal(const std::string& pixelName, TypeNode* type, const std::string& globalName) {
    _globals.emplace(globalName, GlobalEntry{pixelName, type});
}

const GlobalEntry* GlobalRegistry::lookup(const std::string& name) const {
    auto it = _globals.find(name);
    return it != _globals.end() ? &it->second : nullptr;
}
