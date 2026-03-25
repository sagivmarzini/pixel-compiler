//
// Created by sagiv on 23/03/2026.
//

#include "TypeContext.h"

TypeNode* TypeContext::getArray(TypeNode* base, int size) {
    auto key = std::make_pair(base, size);
    auto it = _arrayTypes.find(key);
    if (it != _arrayTypes.end()) return it->second.get();

    auto node = std::make_unique<ArrayTypeNode>(base, size);
    auto* ptr = node.get();
    _arrayTypes.emplace(key, std::move(node));

    return ptr;
}

TypeNode* TypeContext::get(ScalarKind kind) const {
    switch (kind) {
        case ScalarKind::Int: return getInt();
        case ScalarKind::Float: return getFloat();
        case ScalarKind::Bool: return getBool();
        case ScalarKind::String: return getString();
        case ScalarKind::Void: return getVoid();
        default: return nullptr;
    }
}
