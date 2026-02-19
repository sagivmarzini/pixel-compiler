#include "Compiler.h"

#include <format>

#include "semantic/SymbolPool.h"
#include "semantic/SymbolTable.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "IR/IRGeneratorLLVM.h"
#include "parse/AST/AstPrinter.h"
#include "semantic/DeclarationPassVisitor.h"
#include "semantic/TypeCheckerVisitor.h"

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
    // Lexing
    Lexer lexer(_sourceCode);

    const auto tokens = lexer.lex();

    // Parsing
    Parser parser(tokens);
    auto   ast = parser.parseProgram();

    AstPrinter printer;

    // Semantic analyzing
    SymbolPool  symbols;
    SymbolTable symbolTable(symbols);

    DeclarationPassVisitor declPass(symbolTable);
    declPass.run(ast);

    TypeCheckerVisitor typeChecker(symbolTable);
    typeChecker.run(ast);

    IRGeneratorLLVM generator;
    generator.visit(ast);
    generator.print();
}

void Compiler::printTokens(const std::vector<Token>& tokens) {
    for (const auto& token: tokens) {
        std::cout << token << '\n';
    }
}
