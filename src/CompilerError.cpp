//
// Created by Sagiv Marzini on 02/01/2026.
//

#include "CompilerError.h"

CompilerError::CompilerError(const TokenMetadata& metadata)
    : _metadata(metadata) {
}

std::string CompilerError::getMessage() const {
    return _message;
}

TokenMetadata CompilerError::getMetadata() const {
    return _metadata;
}
