//
// Created by Sagiv Marzini on 21/11/2025.
//

#include "CompilerException.h"

#include <format>

CompilerException::CompilerException(const std::vector<CompilerError>& errors) {
    for (const auto& error: errors) {
        const auto [line, col, lexeme] = error.getMetadata();
        _errors.push_back(std::format("{}:{}: error: {}", line, col, error.getMessage()) + '\n');
    }
}

std::vector<std::string> CompilerException::errors() const noexcept {
    return _errors;
}

