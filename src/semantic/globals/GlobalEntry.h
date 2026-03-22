//
// Created by sagiv on 20/03/2026.
//

#ifndef PXL_LANG_GLOBALENTRY_H
#define PXL_LANG_GLOBALENTRY_H
#include <string>

#include "lex/Token.h"

struct GlobalEntry {
    std::string symbolName; // "pxl_mouse_x"
    Type type;
};

#endif //PXL_LANG_GLOBALENTRY_H
