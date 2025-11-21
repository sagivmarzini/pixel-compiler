#include "Compiler.h"

#include <iostream>
#include <utility>

#include "lex/LexerException.h"

Compiler::Compiler(std::string sourceCode) : _sourceCode(std::move(sourceCode)) {
}

void Compiler::compile() const {
    Lexer lexer(_sourceCode);

    try {
        const auto tokens = lexer.lex();
        printTokens(tokens);
    } catch (const LexerException &e) {
        const auto [line, col] = e.location();

        throw CompilerException(std::format("<filename>:{}:{}: {}", line, col, e.what()));
    }
}

void Compiler::printTokens(const std::vector<Token> &tokens) {
    for (const auto &token: tokens) {
        std::cout << token << '\n';
    }
}
