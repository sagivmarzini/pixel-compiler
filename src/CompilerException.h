//
// Created by Sagiv Marzini on 21/11/2025.
//

#ifndef COMPILER_PROJECT_COMPILEREXCEPTION_H
#define COMPILER_PROJECT_COMPILEREXCEPTION_H

#include <exception>
#include <string>

// Generic compiler exception class. Exceptions in specific parts of the compiler inherit from this.
class CompilerException : public std::exception {
public:
    CompilerException() = default;

    CompilerException(std::string msg);

    [[nodiscard]] const char *what() const noexcept override;

protected:
    std::string _msg;
};


#endif //COMPILER_PROJECT_COMPILEREXCEPTION_H
