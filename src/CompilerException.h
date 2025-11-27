//
// Created by Sagiv Marzini on 21/11/2025.
//

#ifndef COMPILER_PROJECT_COMPILEREXCEPTION_H
#define COMPILER_PROJECT_COMPILEREXCEPTION_H

#include <exception>
#include <string>
#include <vector>

#include "lex/LexerError.h"

// Generic compiler error class. Errors in specific parts of the compiler inherit from this.
class CompilerException : public std::exception {
public:
    CompilerException(const std::vector<LexerError> &errors);

    [[nodiscard]] std::vector<std::string> errors() const noexcept;

private:
    std::vector<std::string> _errors;
};


#endif //COMPILER_PROJECT_COMPILEREXCEPTION_H
