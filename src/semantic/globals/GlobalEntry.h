//
// Created by sagiv on 20/03/2026.
//

#ifndef PXL_LANG_GLOBALENTRY_H
#define PXL_LANG_GLOBALENTRY_H
#include <string>

struct TypeNode;

struct GlobalEntry {
    std::string symbolName; // "pxl_mouse_x"
    TypeNode* type;
};

#endif //PXL_LANG_GLOBALENTRY_H
