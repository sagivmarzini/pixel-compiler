//
// Created by Sagiv Marzini on 21/11/2025.
//

#include "CompilerException.h"

CompilerException::CompilerException(std::string msg)
    : _msg(std::move(msg)) {
}

const char *CompilerException::what() const noexcept {
    return _msg.c_str();
}
