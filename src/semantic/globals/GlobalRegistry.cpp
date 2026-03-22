//
// Created by sagiv on 20/03/2026.
//

#include "GlobalRegistry.h"

void GlobalRegistry::registerGlobal(const std::string& pixelName, Type type, const std::string& globalName) {
    _globals[pixelName] = {globalName, type};
}

const GlobalEntry* GlobalRegistry::lookup(const std::string& name) const {
    auto it = _globals.find(name);
    return it != _globals.end() ? &it->second : nullptr;
}
