#include "Compiler.h"

#include <iostream>
#include <utility>

Compiler::Compiler(std::string sourceCode) : _sourceCode(std::move(sourceCode)) {
    std::cout << "Starting compilation\n";

    try {
        compile();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Uncaught exception. Somthing went wrong bro. Good luck, i believe in you.\n";
    }
}

void Compiler::compile() const {
    Lexer lexer(_sourceCode);

    const auto tokens = lexer.lex();
    printTokens(tokens);
}

void Compiler::printTokens(const std::vector<Token> &tokens) {
    for (const auto &token: tokens) {
        std::cout << token << '\n';
    }
}
