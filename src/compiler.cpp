#include "compiler.h"

#include <iostream>
#include <utility>

compiler::compiler(std::string sourceCode) : _sourceCode(std::move(sourceCode)) {
    std::cout << "Starting compilation\n";

    try {
        compile();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Uncaught exception. Somthing went wrong bro. Good luck, i believe in you.\n";
    }
}

void compiler::compile() const {
    lexer lexer(_sourceCode);

    const auto tokens = lexer.lex();
    printTokens(tokens);
}

void compiler::printTokens(const std::vector<token> &tokens) {
    for (const auto &token: tokens) {
        std::cout << token << '\n';
    }
}
