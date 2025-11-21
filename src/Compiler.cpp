#include "Compiler.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "lex/LexerException.h"

Compiler::Compiler(std::string sourceFile) : _sourceFile(std::move(sourceFile)) {
    std::ifstream file(_sourceFile);
    if (!file.is_open()) {
        throw std::invalid_argument(std::format("Failed to open file: '{}'", sourceFile));
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    _sourceCode = buffer.str();
}

void Compiler::compile() const {
    Lexer lexer(_sourceCode);

    try {
        const auto tokens = lexer.lex();
        printTokens(tokens);
    } catch (const LexerException &e) {
        const auto [line, col] = e.location();

        throw CompilerException(std::format("{}:{}:{}: {}", _sourceFile, line, col, e.what()));
    }
}

void Compiler::printTokens(const std::vector<Token> &tokens) {
    for (const auto &token: tokens) {
        std::cout << token << '\n';
    }
}
