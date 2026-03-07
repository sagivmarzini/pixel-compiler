//
// Created by Sagiv Marzini on 01/03/2026.
//

#pragma once
#include <vector>
#include "parse/AST/Statement.h"

struct FunctionSignature {
    std::vector<AST::FunctionDeclaration::FunctionParameter> parameters;
    Type                                                   returnType;
};


