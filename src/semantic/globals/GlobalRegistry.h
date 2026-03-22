//
// Created by sagiv on 20/03/2026.
//

#ifndef PXL_LANG_GLOBALREGISTRY_H
#define PXL_LANG_GLOBALREGISTRY_H
#include <string>

#include "GlobalEntry.h"
#include "lex/Token.h"

class GlobalRegistry {
public:
    void registerGlobal(const std::string& pixelName, Type type, const std::string& globalName);

    const GlobalEntry* lookup(const std::string& name) const;

    const std::unordered_map<std::string, GlobalEntry>& getAllGlobals() const { return _globals; }

private:
    std::unordered_map<std::string, GlobalEntry> _globals;
};


#endif //PXL_LANG_GLOBALREGISTRY_H
